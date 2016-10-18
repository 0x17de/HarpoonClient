#include "HarpoonClient.hpp"
#include <algorithm>
#include <QtCore/QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>
#include <QTime>

#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"

QT_USE_NAMESPACE


HarpoonClient::HarpoonClient()
    : shutdown{false}
    , settings("_0x17de", "HarpoonClient")
{
    connect(&ws_, &QWebSocket::connected, this, &HarpoonClient::onConnected);
    connect(&ws_, &QWebSocket::disconnected, this, &HarpoonClient::onDisconnected);
    connect(&ws_, &QWebSocket::textMessageReceived, this, &HarpoonClient::onTextMessage);
    connect(&ws_, &QWebSocket::binaryMessageReceived, this, &HarpoonClient::onBinaryMessage);
    connect(&reconnectTimer, &QTimer::timeout, this, &HarpoonClient::onReconnectTimer);
    connect(&pingTimer, &QTimer::timeout, this, &HarpoonClient::onPingTimer);

    reconnectTimer.setSingleShot(true);
    harpoonUrl = settings.value("host", "ws://localhost:8080/ws").toString();
}

HarpoonClient::~HarpoonClient() {
    shutdown = true;
}

void HarpoonClient::reconnect(const QString& host) {
    ws_.close();
    harpoonUrl = host;
}

QSettings& HarpoonClient::getSettings() {
    return settings;
}

void HarpoonClient::run() {
    ws_.open(harpoonUrl);
}

void HarpoonClient::onReconnectTimer() {
    ws_.open(harpoonUrl);
}

void HarpoonClient::onPingTimer() {
    qDebug() << "ping";
    ws_.sendTextMessage("{\"cmd\":\"ping\"}");
}

void HarpoonClient::onConnected() {
    qDebug() << "connected";
    ws_.sendTextMessage("LOGIN user password\n");
    pingTimer.start(60000);
}

void HarpoonClient::onDisconnected() {
    pingTimer.stop();
    qDebug() << "disconnected";
    std::list<std::shared_ptr<Server>> empty;
    emit resetServers(empty);
    if (!shutdown)
        reconnectTimer.start(3000);
}

void HarpoonClient::onTextMessage(const QString& message) {
    qDebug() << message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    handleCommand(doc);
}

void HarpoonClient::onBinaryMessage(const QByteArray& data) {
    qDebug() << data;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    handleCommand(doc);
}

void HarpoonClient::sendMessage(Channel* channel, const QString& message) {
    // TODO: only irc works yet.
    if (message.count() == 0)
        return;
    QJsonObject root;
    if (message.at(0) != '/' || (message.count() > 2 && message.at(1) == '/')) {
        root["cmd"] = "chat";
        root["type"] = "irc";
        root["server"] = channel->getServer()->getId();
        root["channel"] = channel->getName();
        root["msg"] = message;
    } else {
        auto parts = message.mid(1).split(' ');
        QString cmd = parts.at(0);
        if (cmd == "")
            return;

        if (cmd == "me") {
            root["cmd"] = "action";
            root["type"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["channel"] = channel->getName();
            root["msg"] = message.mid(cmd.count()+2);
        } else if (cmd == "nick") {
            if (parts.count() < 2)
                return;
            root["cmd"] = "nick";
            root["type"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["nick"] = parts.at(1);
        } else if (cmd == "join") {
            // TODO: join stub
            if (parts.count() < 2)
                return;
            QString channelName = parts.at(1);
            root["cmd"] = "join";
            root["type"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["channel"] = channelName;
            root["password"] = parts.count() == 3 ? parts.at(2) : "";
        } else if (cmd == "part") {
            // TODO: part stub
            QString channelName = parts.count() >= 2 ? parts.at(1) : channel->getName();
            root["cmd"] = "part";
            root["type"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["channel"] = channelName;
        } else {
            return;
        }
    }
    QString json = QJsonDocument{root}.toJson(QJsonDocument::JsonFormat::Compact);
    ws_.sendTextMessage(json);
}

void HarpoonClient::handleCommand(const QJsonDocument& doc) {
    if (!doc.isObject()) return;
    QJsonObject root = doc.object();
    QJsonValue cmdValue = root.value("cmd");
    if (!cmdValue.isString()) return;

    QJsonValue typeValue = root.value("type");
    QString type = typeValue.isString() ? typeValue.toString() : "";

    QString cmd = cmdValue.toString();
    qDebug() << type << ":" << cmd;
    if (type == "") {
        if (cmd == "login") {
            // TODO: ...
        }
    } if (type == "irc") {
        if (cmd == "chatlist") {
            irc_handleChatList(root);
        } else if (cmd == "chat") {
            irc_handleChat(root, false);
        } else if (cmd == "userlist") {
            irc_handleUserList(root);
        } else if (cmd == "nickchange") {
            irc_handleNickChange(root);
        } else if (cmd == "serveradded") {
            irc_handleServerAdded(root);
        } else if (cmd == "topic") {
            irc_handleTopic(root);
        } else if (cmd == "action") {
            irc_handleAction(root);
        } else if (cmd == "kick") {
            irc_handleKick(root);
        } else if (cmd == "notice") {
            irc_handleChat(root, true);
        } else if (cmd == "join") {
            irc_handleJoin(root);
        } else if (cmd == "part") {
            irc_handlePart(root);
        } else if (cmd == "quit") {
            irc_handleQuit(root);
        }
    }
}

QString HarpoonClient::formatTimestamp(double timestamp) {
    return QTime{QDateTime{QDateTime::fromTime_t(timestamp/1000)}.time()}.toString("[hh:mm:ss]");
}

void HarpoonClient::irc_handleServerAdded(const QJsonObject& root) {
    auto serverIdValue = root.value("server");
    auto nameValue = root.value("name");

    if (!serverIdValue.isString()) return;
    if (!nameValue.isString()) return;

    QString serverId = serverIdValue.toString();
    QString name = nameValue.toString();

    // no nick yet, also inactive
    emit newServer(std::make_shared<Server>("", serverId, name, true));
}

void HarpoonClient::irc_handleTopic(const QJsonObject& root) {
    //auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    auto nickValue = root.value("nick");
    auto topicValue = root.value("topic");

    //if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;
    if (!nickValue.isString()) return;
    if (!topicValue.isString()) return;

    //size_t id;
    //istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    QString nick = nickValue.toString();
    QString topic = topicValue.toString();

    emit topicChanged(serverId, channelName, time, nick, topic);
}

void HarpoonClient::irc_handleUserList(const QJsonObject& root) {
    //auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    auto usersValue = root.value("users");

    //if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;
    if (!usersValue.isArray()) return;

    //size_t id;
    //istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    auto users = usersValue.toArray();

    std::list<std::shared_ptr<User>> userList;
    for (auto userEntry : users) {
        if (!userEntry.isString()) return;
        userList.push_back(std::make_shared<User>(userEntry.toString()));
    }
    emit resetUsers(serverId, channelName, userList);
}

void HarpoonClient::irc_handleJoin(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;

    size_t id;
    istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    emit joinChannel(serverId, channelName, time, nick);
}

void HarpoonClient::irc_handlePart(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;

    size_t id;
    istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    emit partChannel(serverId, channelName, time, nick);
}

void HarpoonClient::irc_handleNickChange(const QJsonObject& root) {
    //auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto newNickValue = root.value("newNick");
    auto serverIdValue = root.value("server");

    //if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!newNickValue.isString()) return;
    if (!serverIdValue.isString()) return;

    //size_t id;
    //istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString newNick = newNickValue.toString();
    QString serverId = serverIdValue.toString();

    emit nickChange(serverId, time, nick, newNick);
}

void HarpoonClient::irc_handleKick(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    auto targetValue = root.value("target");
    auto reasonValue = root.value("msg");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;
    if (!targetValue.isString()) return;
    if (!reasonValue.isString()) return;

    size_t id;
    istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    QString target = targetValue.toString();
    QString reason = reasonValue.toString();

    emit userKicked(serverId, channelName, time, nick, target, reason);
}

void HarpoonClient::irc_handleQuit(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto serverIdValue = root.value("server");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!serverIdValue.isString()) return;

    size_t id;
    istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();

    emit quitServer(serverId, time, nick);
}

void HarpoonClient::irc_handleChat(const QJsonObject& root, bool notice) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto messageValue = root.value("msg");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!messageValue.isString()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;

    size_t id;
    istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString message = messageValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    emit chatMessage(serverId, channelName, time, nick, message, notice);
}

void HarpoonClient::irc_handleAction(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto messageValue = root.value("msg");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!messageValue.isString()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;

    size_t id;
    istringstream(idValue.toString()) >> id;
    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString message = messageValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    emit chatAction(serverId, channelName, time, nick, message);
}

void HarpoonClient::irc_handleChatList(const QJsonObject& root) {
    std::list<std::shared_ptr<Server>> serverList;

    QJsonValue serversValue = root.value("servers");
    if (!serversValue.isObject()) return;

    QJsonObject servers = serversValue.toObject();
    for (auto sit = servers.begin(); sit != servers.end(); ++sit) {
        QString serverId = sit.key();
        QJsonValueRef serverValue = sit.value();
        if (!serverValue.isObject()) return;

        QJsonObject server = serverValue.toObject();


        QJsonValue serverNameValue = server.value("name");
        if (!serverNameValue.isString()) return;
        QString serverName = serverNameValue.toString();

        QJsonValue activeNickValue = server.value("nick");
        if (!activeNickValue.isString()) return;
        QString activeNick = activeNickValue.toString();


        QJsonValue channelsValue = server.value("channels");
        if (!channelsValue.isObject()) return;

        auto currentServer = std::make_shared<Server>(activeNick, serverId, serverName, false); // TODO: server needs to send if status is disabled
        serverList.push_back(currentServer);

        QJsonObject channels = channelsValue.toObject();
        for (auto cit = channels.begin(); cit != channels.end(); ++cit) {
            QString channelName = cit.key();
            QJsonValueRef channelValue = cit.value();
            if (!channelValue.isObject()) return;
            auto channelData = channelValue.toObject();
            auto channelDisabledValue = channelData.value("disabled");
            bool channelDisabled = channelDisabledValue.isBool() && channelDisabledValue.toBool();

            auto currentChannel = std::make_shared<Channel>(currentServer.get(), channelName, channelDisabled);
            currentServer->addChannel(currentChannel);

            QJsonObject channel = channelValue.toObject();
            QJsonValue usersValue = channel.value("users");
            if (!usersValue.isObject()) return;

            std::list<std::shared_ptr<User>> userList;
            QJsonObject users = usersValue.toObject();
            for (auto uit = users.begin(); uit != users.end(); ++uit) {
                QString nick = uit.key();
                userList.push_back(std::make_shared<User>(nick));
            }

            currentChannel->resetUsers(userList);
        }
    }

    emit resetServers(serverList);
}
