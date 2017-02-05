#include "HarpoonClient.hpp"
#include "moc_HarpoonClient.cpp"

#include "Server.hpp"
#include "models/ServerTreeModel.hpp"
#include "models/SettingsTypeModel.hpp"
#include "Host.hpp"
#include "Channel.hpp"
#include "User.hpp"

#include <algorithm>
#include <sstream>
#include <QDebug>
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
    connect(&serverTreeModel, &ServerTreeModel::newChannel, this, &HarpoonClient::onNewChannel);

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

void HarpoonClient::onNewChannel(std::shared_ptr<Channel> channel) {
    connect(channel.get(), &Channel::backlogRequest, this, &HarpoonClient::backlogRequest);
}

void HarpoonClient::backlogRequest(Channel* channel) {
    // TODO: handle backlog request
    //size_t firstId = channel->getFirstId();
}

void HarpoonClient::sendMessage(Server* server, Channel* channel, const QString& message) {
    // TODO: only irc works yet.
    if (message.count() == 0)
        return; // don't send empty messages

    QJsonObject root;

    if (message.at(0) != '/' || (message.count() > 2 && message.at(1) == '/')) {
        if (!server || !channel)
            return; // regular messages are only available in channels

        root["cmd"] = "chat";
        root["protocol"] = "irc";
        root["server"] = server->getId();
        root["channel"] = channel->getName();
        root["msg"] = message;
    } else {
        auto parts = message.mid(1).split(' ');
        QString cmd = parts.at(0);
        if (cmd == "")
            return;

        if (cmd == "reconnect") { // reconnect to server
            // cmd [serverId]
            root["cmd"] = "reconnect";
            root["protocol"] = "irc";
            QString serverId = parts.count() >= 2 ? parts.at(1) : server->getId();
            root["server"] = server->getId();
        } else if (cmd == "deleteserver") { // remove server
            // cmd [serverId]
            root["cmd"] = "deleteserver";
            root["protocol"] = "irc";
            QString serverId = parts.count() >= 2 ? parts.at(1) : server->getId();
            root["server"] = server->getId();
        } else if (cmd == "editserver") { // add new server
            if (parts.count() < 2) // cmd [oldname] newname
                return;

            root["cmd"] = "editserver";
            root["protocol"] = "irc";
            QString serverId = parts.count() >= 3 ? parts.at(1) : server->getId();
            QString serverName = parts.count() >= 3 ? parts.at(2) : parts.at(1);
            root["server"] = serverId;
            root["name"] = serverName;
        } else if (cmd == "addserver") { // add new server
            if (parts.count() < 2) // cmd name
                return;

            root["cmd"] = "addserver";
            root["protocol"] = "irc";
            root["name"] = parts.at(1);
        } else if (cmd == "addhost") {
            if (parts.count() < 5) // cmd name
                return;

            root["cmd"] = "addhost";
            root["protocol"] = "irc";

            QString serverId = server->getId();
            QString host = parts.at(1);
            QString port = parts.at(2);
            QString ssl = parts.at(3);
            QString ipv6 = parts.at(4);

            root["server"] = serverId;
            root["host"] = host;
            root["port"] = port.toInt();
            root["ssl"] = ssl != "false" && ssl != "0";
            root["ipv6"] = ipv6 != "false" && ipv6 != "0";
        } else if (cmd == "edithost") {
            if (parts.count() < 7) // cmd name
                return;

            root["cmd"] = "modifyhost";
            root["protocol"] = "irc";

            QString serverId = server->getId();
            QString oldHost = parts.at(1);
            QString oldPort = parts.at(2);
            QString host = parts.at(3);
            QString port = parts.at(4);
            QString ssl = parts.at(5) ;
            QString ipv6 = parts.at(6);

            root["server"] = serverId;
            root["oldhost"] = oldHost;
            root["oldport"] = oldPort.toInt();
            root["host"] = host;
            root["port"] = port.toInt();
            root["ssl"] = ssl != "false" && ssl != "0";
            root["ipv6"] = ipv6 != "false" && ipv6 != "0";
        } else if (cmd == "deletehost") {
            if (parts.count() < 3) // cmd name
                return;

            root["cmd"] = "deletehost";
            root["protocol"] = "irc";

            QString serverId = server->getId();
            QString host = parts.at(1);
            QString port = parts.at(2);

            root["server"] = serverId;
            root["host"] = host;
            root["port"] = port.toInt();
        } else if (cmd == "addnick") { // add nick
            if (parts.count() < 2) // cmd [serverId] oldnick newnick
                return;

            QString serverId = parts.count() == 3 ? parts.at(1) : server->getId();
            QString newNick = parts.at(parts.count() == 2 ? 1 : 2);

            root["cmd"] = "modifynick";
            root["protocol"] = "irc";
            root["server"] = serverId;
            root["oldnick"] = "";
            root["newnick"] = newNick;
        } else if (cmd == "deletenick") { // delete nick
            if (parts.count() < 2) // cmd [serverId] oldnick newnick
                return;

            QString serverId = parts.count() == 3 ? parts.at(1) : server->getId();
            QString deleteNick = parts.at(parts.count() == 2 ? 1 : 2);

            root["cmd"] = "modifynick";
            root["protocol"] = "irc";
            root["server"] = serverId;
            root["oldnick"] = deleteNick;
            root["newnick"] = "";
        } else if (cmd == "editnick") { // modify nick
            if (parts.count() < 3) // cmd [serverId] oldnick newnick
                return;

            QString serverId = parts.count() == 4 ? parts.at(1) : server->getId();
            QString oldNick = parts.at(parts.count() == 3 ? 1 : 2);
            QString newNick = parts.at(parts.count() == 3 ? 2 : 3);

            root["cmd"] = "modifynick";
            root["protocol"] = "irc";
            root["server"] = serverId;
            root["oldnick"] = oldNick;
            root["newnick"] = newNick;
        } else if (channel != nullptr) { // channel commands
            if (cmd == "me") {
                root["cmd"] = "action";
                root["protocol"] = "irc";
                root["server"] = server->getId();
                root["channel"] = channel->getName();
                root["msg"] = message.mid(cmd.count()+2);
            } else if (cmd == "nick") { // change nick
                if (parts.count() < 2)
                    return;

                root["cmd"] = "nick";
                root["protocol"] = "irc";
                root["server"] = server->getId();
                root["nick"] = parts.at(1);
            } else if (cmd == "join") { // join channel
                QString channelName = parts.count() >= 2 ? parts.at(1) : channel->getName();
                root["cmd"] = "join";
                root["protocol"] = "irc";
                root["server"] = server->getId();
                root["channel"] = channelName;
                root["password"] = parts.count() == 3 ? parts.at(2) : "";
            } else if (cmd == "part") { // leave channel
                QString channelName = parts.count() >= 2 ? parts.at(1) : channel->getName();
                root["cmd"] = "part";
                root["protocol"] = "irc";
                root["server"] = server->getId();
                root["channel"] = channelName;
            } else if (cmd == "deletechannel") { // delete channel
                QString channelName = parts.count() >= 2 ? parts.at(1) : channel->getName();
                root["cmd"] = "deletechannel";
                root["protocol"] = "irc";
                root["server"] = server->getId();
                root["channel"] = channelName;
            } else {
                return; // nothing is sent
            }
        } else {
            return; // nothing is sent
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
        } else if (cmd == "userlist") {
            irc_handleUserList(root);
        } else if (cmd == "nickchange") {
            irc_handleNickChange(root);
        } else if (cmd == "nickmodified") {
            irc_handleNickModified(root);
        } else if (cmd == "serveradded") {
            irc_handleServerAdded(root);
        } else if (cmd == "serverremoved") {
            irc_handleServerDeleted(root);
        } else if (cmd == "hostadded") {
            irc_handleHostAdded(root);
        } else if (cmd == "hostdeleted") {
            irc_handleHostDeleted(root);
        } else if (cmd == "topic") {
            irc_handleTopic(root);
        } else if (cmd == "action") {
            irc_handleAction(root);
        } else if (cmd == "mode") {
            irc_handleMode(root);
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

        std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);

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

            bool ssl = sslValue.toBool();
            bool ipv6 = ipv6Value.toBool();

            std::shared_ptr<Host> newHost{std::make_shared<Host>(server, hostname, port, ssl, ipv6)};
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
    //auto hasPasswordValue = root.value("hasPassword");
    auto portValue = root.value("port");
    auto sslValue = root.value("ssl");
    auto ipv6Value = root.value("ipv6");

    if (!serverIdValue.isString()) return;
    if (!hostValue.isString()) return;
    //if (!hasPasswordValue.isString()) return;
    if (!portValue.isDouble()) return;
    if (!sslValue.isBool()) return;
    if (!ipv6Value.isBool()) return;

    QString serverId = serverIdValue.toString();
    QString hostName = hostValue.toString();
    //bool hasPassword = hasPasswordValue.toBool();
    int port = portValue.toInt();
    bool ssl = sslValue.toBool();
    bool ipv6 = ipv6Value.toBool();

    // TODO: has password

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    auto host = std::make_shared<Host>(server, hostName, port, ssl, ipv6);
    server->getHostModel().newHost(host);
}

void HarpoonClient::irc_handleHostDeleted(const QJsonObject& root) {
    auto serverIdValue = root.value("server");
    auto hostValue = root.value("host");
    auto portValue = root.value("port");

    if (!serverIdValue.isString()) return;
    if (!hostValue.isString()) return;
    if (!portValue.isDouble()) return;

    QString serverId = serverIdValue.toString();
    QString host = hostValue.toString();
    int port = portValue.toInt();

    auto server = serverTreeModel_.getServer(serverId);
    server->getHostModel().deleteHost(host, port);
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

    auto server = serverTreeModel_.getServer(serverId);
    auto* channel = server->getChannelModel().getChannel(channelName);
    channel->setTopic(id, time, nick, topic);
    emit topicChanged(channel, topic);
}

void HarpoonClient::irc_handleUserList(const QJsonObject& root) {
    //auto idValue = root.value("id");
    //auto timeValue = root.value("time");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    auto usersValue = root.value("users");

    //if (!idValue.isString()) return;
    //if (!timeValue.isDouble()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;
    if (!usersValue.isObject()) return;

    //size_t id;
    //std::istringstream(idValue.toString().toStdString()) >> id;
    //double time = timeValue.toDouble();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    auto users = usersValue.toObject();

    std::list<std::shared_ptr<User>> userList;
    for (auto userEntryIt = users.begin(); userEntryIt != users.end(); ++userEntryIt) {
        auto username = userEntryIt.key();
        auto modeValue = userEntryIt.value();
        if (!modeValue.isString()) continue;
        auto mode = modeValue.toString();
        userList.push_back(std::make_shared<User>(username, mode));
    }

    auto server = serverTreeModel_.getServer(serverId);
    server->getChannelModel().getChannel(channelName)->getUserModel().resetUsers(userList);
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

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    auto& channelModel = server->getChannelModel();
    Channel* channel = channelModel.getChannel(channelName);

    if (User::stripNick(nick) == server->getActiveNick()) {
        if (channel != nullptr) {
            channel->setDisabled(false);
        } else {
            std::shared_ptr<Channel> channelPtr{std::make_shared<Channel>(0 /* backlog last id */, server, channelName, false)};
            channel = channelPtr.get();
            channelModel.addChannel(channelPtr);
        }
    }
    if (channel) {
        channel->addMessage(id, time, "-->", User::stripNick(nick) + " joined the channel", MessageColor::Event);
        channel->getUserModel().addUser(std::make_shared<User>(nick));
    }
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

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    auto& channelModel = server->getChannelModel();
    Channel* channel = channelModel.getChannel(channelName);

    if (User::stripNick(nick) == server->getActiveNick()) {
        if (channel != nullptr) {
            channel->setDisabled(true);
        } else {
            std::shared_ptr<Channel> channelPtr{std::make_shared<Channel>(0 /* backlog last id */, server, channelName, true)};
            channel = channelPtr.get();
            channelModel.addChannel(channelPtr);
        }
    }
    if (channel) {
        channel->addMessage(id, time, "<--", User::stripNick(nick) + " left the channel", MessageColor::Event);
        channel->getUserModel().removeUser(User::stripNick(nick));
    }
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

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    if (server == nullptr) return;

    if (server->getActiveNick() == nick)
        server->setActiveNick(newNick);

    for (auto& channel : server->getChannelModel().getChannels()) {
        if (channel->getUserModel().renameUser(User::stripNick(nick), newNick))
            channel->getBacklogView()->addMessage(id, time, "<->", User::stripNick(nick) + " is now known as " + newNick, MessageColor::Event);
    }
}

void HarpoonClient::irc_handleNickModified(const QJsonObject& root) {
    //auto timeValue = root.value("time");
    auto serverIdValue = root.value("server");
    auto oldNickValue = root.value("oldnick");
    auto newNickValue = root.value("newnick");

    //if (!timeValue.isDouble()) return;
    if (!serverIdValue.isString()) return;
    if (!oldNickValue.isString()) return;
    if (!newNickValue.isString()) return;

    //double time = timeValue.toDouble();
    QString serverId = serverIdValue.toString();
    QString oldNick = oldNickValue.toString();
    QString newNick = newNickValue.toString();

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    if (server == nullptr) return;

    if (server->getActiveNick() == oldNick)
        server->setActiveNick(newNick);

    server->getNickModel().modifyNick(oldNick, newNick);
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

    auto server = serverTreeModel_.getServer(serverId);
    Channel* channel = server->getChannelModel().getChannel(channelName);
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

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
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

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    Channel* channel = server->getChannelModel().getChannel(channelName);
    if (!channel) return;
    channel->addMessage(id, time, "*", User::stripNick(nick) + " " + message, MessageColor::Action);
}

void HarpoonClient::irc_handleMode(const QJsonObject& root) {
    auto idValue = root.value("id");
    auto timeValue = root.value("time");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    auto nickValue = root.value("nick");
    auto modeValue = root.value("mode");
    auto argsValue = root.value("args");

    if (!idValue.isString()) return;
    if (!timeValue.isDouble()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;
    if (!nickValue.isString()) return;
    if (!modeValue.isString()) return;
    if (!argsValue.isArray()) return;

    size_t id;
    std::istringstream(idValue.toString().toStdString()) >> id;
    double time = timeValue.toDouble();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();
    QString nick = nickValue.toString();
    QString mode = modeValue.toString();
    auto args = argsValue.toArray();

    std::shared_ptr<Server> server = serverTreeModel_.getServer(serverId);
    Channel* channel = server->getChannelModel().getChannel(channelName);
    if (!channel) return;

    bool add = true;
    size_t userIndex = 0;
    for (QChar c : mode) {
        char modeChar = c.toLatin1();
        if (modeChar == '+') {
            add = true;
        } else if (modeChar == '-') {
            add = false;
        } else {
            if (userIndex > args.count()) break;
            auto nickTargetValue = args.at(userIndex);
            QString nickTarget = nickTargetValue.toString();

            channel->getUserModel().changeMode(nickTarget, modeChar, add);
            channel->addMessage(id,
                                time,
                                "*",
                                User::stripNick(nick) + " sets mode "
                                  + (add ? '+' : '-') + QChar(modeChar)
                                  + " on " + nickTarget,
                                MessageColor::Event);
            userIndex += 1;
        }
    }
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

            auto currentChannel = std::make_shared<Channel>(firstId, currentServer, channelName, channelDisabled);
            currentServer->getChannelModel().addChannel(currentChannel);

            QJsonObject channel = channelValue.toObject();
            QJsonValue usersValue = channel.value("users");
            if (!usersValue.isObject()) return;

            std::list<std::shared_ptr<User>> userList;
            QJsonObject users = usersValue.toObject();
            for (auto uit = users.begin(); uit != users.end(); ++uit) {
                QString nick = uit.key();
                auto modeValue = uit.value();
                if (!modeValue.isString()) continue;
                auto mode = modeValue.toString();
                userList.push_back(std::make_shared<User>(nick, mode));
            }

            currentChannel->resetUsers(userList);
        }
    }
    serverTreeModel_.resetServers(serverList);
}
