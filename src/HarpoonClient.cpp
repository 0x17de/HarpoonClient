#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "models/ServerTreeModel.hpp"
#include "models/SettingsTypeModel.hpp"
#include "Host.hpp"
#include "Channel.hpp"
#include "User.hpp"

#include <algorithm>
#include <sstream>
#include <QtCore/QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

QT_USE_NAMESPACE


HarpoonClient::HarpoonClient(ServerTreeModel& serverTreeModel,
                             SettingsTypeModel& settingsTypeModel)
    : shutdown_{false}
    , serverTreeModel_{serverTreeModel}
    , settingsTypeModel_{settingsTypeModel}
    , settings_("_0x17de", "HarpoonClient")
{
    connect(&ws_, &QWebSocket::connected, this, &HarpoonClient::onConnected);
    connect(&ws_, &QWebSocket::textMessageReceived, this, &HarpoonClient::onTextMessage);
    connect(&ws_, &QWebSocket::binaryMessageReceived, this, &HarpoonClient::onBinaryMessage);
    connect(&reconnectTimer_, &QTimer::timeout, this, &HarpoonClient::onReconnectTimer);
    connect(&pingTimer_, &QTimer::timeout, this, &HarpoonClient::onPingTimer);

    reconnectTimer_.setSingleShot(true);
    username_ = settings_.value("username", "user").toString();
    password_ = settings_.value("password", "password").toString();
    harpoonUrl_ = settings_.value("host", "ws://localhost:8080/ws").toString();
}

HarpoonClient::~HarpoonClient() {
    shutdown_ = true;
}

void HarpoonClient::reconnect(const QString& lusername,
                              const QString& lpassword,
                              const QString& host) {
    qDebug() << "reconnect";
    ws_.close();
    username_ = lusername;
    password_ = lpassword;
    harpoonUrl_ = host;
}

QSettings& HarpoonClient::getSettings() {
    return settings_;
}

void HarpoonClient::run() {
    ws_.open(harpoonUrl_);
}

void HarpoonClient::onReconnectTimer() {
    ws_.open(harpoonUrl_);
}

void HarpoonClient::onPingTimer() {
    qDebug() << "ping";
    ws_.sendTextMessage("{\"cmd\":\"ping\"}");
}

void HarpoonClient::onConnected() {
    qDebug() << "connected";
    QString loginCommand = QString("LOGIN ") + username_ + " " + password_ + "\n";
    ws_.sendTextMessage(loginCommand);
    pingTimer_.start(60000);
}

void HarpoonClient::onDisconnected() {
    pingTimer_.stop();
    qDebug() << "disconnected";
    std::list<std::shared_ptr<Server>> emptyServerList;
    serverTreeModel_.resetServers(emptyServerList);
    std::list<QString> emptyTypeList;
    settingsTypeModel_.resetTypes(emptyTypeList);
    if (!shutdown_)
        reconnectTimer_.start(3000);
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

void HarpoonClient::backlogRequest(Channel* channel) {
    // TODO: handle backlog request
    size_t firstId = channel->getFirstId();
}

void HarpoonClient::sendMessage(Channel* channel, const QString& message) {
    // TODO: only irc works yet.
    if (message.count() == 0)
        return;
    QJsonObject root;
    if (message.at(0) != '/' || (message.count() > 2 && message.at(1) == '/')) {
        root["cmd"] = "chat";
        root["protocol"] = "irc";
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
            root["protocol"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["channel"] = channel->getName();
            root["msg"] = message.mid(cmd.count()+2);
        } else if (cmd == "nick") {
            if (parts.count() < 2)
                return;
            root["cmd"] = "nick";
            root["protocol"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["nick"] = parts.at(1);
        } else if (cmd == "join") {
            // TODO: join stub
            if (parts.count() < 2)
                return;
            QString channelName = parts.at(1);
            root["cmd"] = "join";
            root["protocol"] = "irc";
            root["server"] = channel->getServer()->getId();
            root["channel"] = channelName;
            root["password"] = parts.count() == 3 ? parts.at(2) : "";
        } else if (cmd == "part") {
            // TODO: part stub
            QString channelName = parts.count() >= 2 ? parts.at(1) : channel->getName();
            root["cmd"] = "part";
            root["protocol"] = "irc";
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

    QJsonValue typeValue = root.value("protocol");
    QString type = typeValue.isString() ? typeValue.toString() : "";

    QString cmd = cmdValue.toString();
    qDebug() << type << ":" << cmd;
    if (type == "") {
        if (cmd == "login") {
            handleLogin(root);
        }
    } if (type == "irc") {
        if (cmd == "chatlist") {
            irc_handleChatList(root);
        } else if (cmd == "chat") {
            irc_handleChat(root, false);
        } else if (cmd == "models") {
            irc_handleUserList(root);
        } else if (cmd == "nickchange") {
            irc_handleNickChange(root);
        } else if (cmd == "serveradded") {
            irc_handleServerAdded(root);
        } else if (cmd == "serverremoved") {
            irc_handleServerDeleted(root);
        } else if (cmd == "hostadded") {
            irc_handleHostAdded(root);
        } else if (cmd == "hostremoved") {
            irc_handleHostDeleted(root);
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
        } else if (cmd == "settings") {
            irc_handleSettings(root);
        } else if (cmd == "quit") {
            irc_handleQuit(root);
        }
    }
}

void HarpoonClient::handleLogin(const QJsonObject& root) {
    auto successValue = root.value("success");
    if (!successValue.isBool()) return;
    bool success = successValue.toBool();
    if (success) {
        QJsonObject newRoot;
        newRoot["cmd"] = "querysettings";
        QString json = QJsonDocument{newRoot}.toJson(QJsonDocument::JsonFormat::Compact);
        ws_.sendTextMessage(json);
    } else {
        // TODO
    }
}

void HarpoonClient::irc_handleSettings(const QJsonObject& root) {
    // TODO: nicks, hasPassword, ipv6, ssl

    auto dataValue = root["data"];
    if (!dataValue.isObject()) return;
    QJsonObject data = dataValue.toObject();

    auto serversValue = data["servers"];
    if (!serversValue.isObject()) return;
    auto servers = serversValue.toObject();

    for (auto serverIt = servers.begin(); serverIt != servers.end(); ++serverIt) {
        QString serverId = serverIt.key();
        auto serverDataValue = serverIt.value();
        if (!serverDataValue.isObject()) return;
        auto serverData = serverDataValue.toObject();

        Server* server = serverTreeModel_.getServer(serverId);

        auto hostsValue = serverData["hosts"];
        auto nicksValue = serverData["nicks"];
        if (!hostsValue.isObject()) return;
        if (!nicksValue.isArray()) return;
        auto hosts = hostsValue.toObject();
        auto nicks = nicksValue.toArray();

        std::list<std::shared_ptr<Host>> newHosts;
        std::list<QString> newNicks;

        for (auto hostIt = hosts.begin(); hostIt != hosts.end(); ++hostIt) {
            QString hostKey = hostIt.key();
            auto hostDataValue = hostIt.value();
            if (!hostDataValue.isObject()) return;
            auto hostData = hostDataValue.toObject();

            auto hasPasswordValue = hostData["hasPassword"];
            auto ipv6Value = hostData["ipv6"];
            auto sslValue = hostData["ssl"];

            if (!hasPasswordValue.isBool()) return;
            if (!ipv6Value.isBool()) return;
            if (!sslValue.isBool()) return;

            int colonPosition = hostKey.indexOf(":");
            if (colonPosition == -1) return;
            QString hostname = hostKey.left(colonPosition);
            int port = hostKey.right(hostKey.size() - colonPosition - 1).toInt();

            std::shared_ptr<Host> newHost{std::make_shared<Host>(server, hostname, port)};
            newHosts.push_back(newHost);
        }

        for (auto nickIt = nicks.begin(); nickIt != nicks.end(); ++nickIt) {
            auto nickValue = *nickIt;
            if (!nickValue.isString()) return;
            newNicks.push_back(nickValue.toString());
        }

        server->getHostModel().resetHosts(newHosts);
        server->getNickModel().resetNicks(newNicks);
    }

    settingsTypeModel_.newType("irc");
}

void HarpoonClient::irc_handleServerAdded(const QJsonObject& root) {
    auto serverIdValue = root.value("server");
    auto nameValue = root.value("name");

    if (!serverIdValue.isString()) return;
    if (!nameValue.isString()) return;

    QString serverId = serverIdValue.toString();
    QString name = nameValue.toString();

    // no nick yet, also inactive
    // TODO: firstId for new servers

    auto server = std::make_shared<Server>("", serverId, name, true);
    serverTreeModel_.newServer(server);
}

void HarpoonClient::irc_handleServerDeleted(const QJsonObject& root) {
    auto serverIdValue = root.value("server");

    if (!serverIdValue.isString()) return;

    QString serverId = serverIdValue.toString();
    serverTreeModel_.deleteServer(serverId);
}

void HarpoonClient::irc_handleHostAdded(const QJsonObject& root) {
    auto serverIdValue = root.value("server");
    auto hostValue = root.value("host");
    auto hasPasswordValue = root.value("hasPassword");
    auto portValue = root.value("port");
    auto ipv6Value = root.value("ipv6");
    auto sslValue = root.value("ssl");

    if (!serverIdValue.isString()) return;
    if (!hostValue.isString()) return;
    if (!hasPasswordValue.isString()) return;
    if (!portValue.isString()) return;
    if (!ipv6Value.isString()) return;
    if (!sslValue.isString()) return;

    QString serverId = serverIdValue.toString();
    QString hostName = hostValue.toString();
    bool hasPassword = hostValue.toBool();
    int port = hostValue.toInt();
    bool ipv6 = hostValue.toBool();
    bool ssl = hostValue.toBool();

    Server* server = serverTreeModel_.getServer(serverId);
    auto host = std::make_shared<Host>(server, hostName, port);
    server->getHostModel().newHost(host);
}

void HarpoonClient::irc_handleHostDeleted(const QJsonObject& root) {
    auto serverIdValue = root.value("server");
    auto hostValue = root.value("host");
    auto portValue = root.value("port");

    if (!serverIdValue.isString()) return;
    if (!hostValue.isString()) return;
    if (!portValue.isString()) return;

    QString serverId = serverIdValue.toString();
    QString host = hostValue.toString();
    int port = hostValue.toInt();

    serverTreeModel_.getServer(serverId)->getHostModel().deleteHost(host, port);
}

void HarpoonClient::irc_handleTopic(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    auto nickValue = root.value("nick");
    auto topicValue = root.value("topic");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;
    if (!nickValue.isString()) return;
    if (!topicValue.isString()) return;

    size_t id;
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    QString nick = nickValue.toString();
    QString topic = topicValue.toString();

    auto* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
    channel->setTopic(id, time, nick, topic);
    emit topicChanged(channel, topic);
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
    //std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    auto users = usersValue.toArray();

    std::list<std::shared_ptr<User>> userList;
    for (auto userEntry : users) {
        if (!userEntry.isString()) return;
        userList.push_back(std::make_shared<User>(userEntry.toString()));
    }

    serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName)->getUserModel().resetUsers(userList);
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
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    Server* server = serverTreeModel_.getServer(serverId);
    auto& channelModel = server->getChannelModel();
    Channel* channel = channelModel.getChannel(channelName);

    if (server->getActiveNick() == User::stripNick(nick)) {
        if (channel != nullptr) {
            channel->setDisabled(false);
        } else {
            std::shared_ptr<Channel> channelPtr{std::make_shared<Channel>(0 /* backlog last id */, server, channelName, false)};
            channel = channelPtr.get();
            channelModel.newChannel(channelPtr);
        }
    }
    channel->addMessage(id, time, "-->", User::stripNick(nick) + " joined the channel", MessageColor::Event);
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
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    Server* server = serverTreeModel_.getServer(serverId);
    auto& channelModel = server->getChannelModel();
    Channel* channel = channelModel.getChannel(channelName);

    if (server->getActiveNick() == User::stripNick(nick)) {
        if (channel != nullptr) {
            channel->setDisabled(true);
        } else {
            std::shared_ptr<Channel> channelPtr{std::make_shared<Channel>(0 /* backlog last id */, server, channelName, true)};
            channel = channelPtr.get();
            channelModel.newChannel(channelPtr);
        }
    }
    channel->addMessage(id, time, "<--", User::stripNick(nick) + " left the channel", MessageColor::Event);
}

void HarpoonClient::irc_handleNickChange(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto newNickValue = root.value("newNick");
    auto serverIdValue = root.value("server");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!newNickValue.isString()) return;
    if (!serverIdValue.isString()) return;

    size_t id;
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString newNick = newNickValue.toString();
    QString serverId = serverIdValue.toString();

    Server* server = serverTreeModel_.getServer(serverId);
    if (server == nullptr) return;

    if (server->getActiveNick() == nick)
        server->setActiveNick(newNick);

    for (auto& channel : server->getChannelModel().getChannels()) {
        if (channel->getUserModel().renameUser(User::stripNick(nick), newNick))
            channel->getBacklogView()->addMessage(id, time, "<->", User::stripNick(nick) + " is now known as " + newNick, MessageColor::Event);
    }
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
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    QString target = targetValue.toString();
    QString reason = reasonValue.toString();

    Channel* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
    if (channel == nullptr) return;
    channel->getUserModel().removeUser(User::stripNick(nick));
    channel->getBacklogView()->addMessage(id, time, "<--", nick + " was kicked (Reason: " + reason + ")", MessageColor::Event);
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
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString serverId = serverIdValue.toString();

    for (auto& server : serverTreeModel_.getServers()) {
        for (auto& channel : server->getChannelModel().getChannels()) {
            if (channel->getUserModel().removeUser(User::stripNick(nick)))
                channel->getBacklogView()->addMessage(id, time, "<--", nick + " has quit", MessageColor::Event);
        }
    }
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
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString message = messageValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    Server* server = serverTreeModel_.getServer(serverId);
    if (!server) return;
    Channel* channel = server->getChannelModel().getChannel(channelName);
    if (!channel) return;
    channel->addMessage(id, time, '<'+User::stripNick(nick)+'>', message, MessageColor::Default);
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
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString nick = nickValue.toString();
    QString message = messageValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    Server* server = serverTreeModel_.getServer(serverId);
    if (!server) return;
    Channel* channel = server->getChannelModel().getChannel(channelName);
    if (!channel) return;
    channel->addMessage(id, time, "*", User::stripNick(nick) + " " + message, MessageColor::Action);
}

void HarpoonClient::irc_handleChatList(const QJsonObject& root) {
    std::list<std::shared_ptr<Server>> serverList;

    QJsonValue firstIdValue = root.value("firstId");
    if (!firstIdValue.isString()) return;
    size_t firstId;
    std::istringstream(firstIdValue.toString().toStdString()) >> firstId;

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

            auto currentChannel = std::make_shared<Channel>(firstId, currentServer.get(), channelName, channelDisabled);
            currentServer->getChannelModel().newChannel(currentChannel);

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
    serverTreeModel_.resetServers(serverList);
}
