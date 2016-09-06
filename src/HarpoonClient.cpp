#include "HarpoonClient.hpp"
#include <algorithm>
#include <QtCore/QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QTime>

#include "Server.hpp"
#include "Channel.hpp"

QT_USE_NAMESPACE


HarpoonClient::HarpoonClient()
    : shutdown{false}
{
    connect(&ws_, &QWebSocket::connected, this, &HarpoonClient::onConnected);
    connect(&ws_, &QWebSocket::disconnected, this, &HarpoonClient::onDisconnected);
    connect(&ws_, &QWebSocket::textMessageReceived, this, &HarpoonClient::onTextMessage);
    connect(&ws_, &QWebSocket::binaryMessageReceived, this, &HarpoonClient::onBinaryMessage);
    connect(&reconnectTimer, &QTimer::timeout, this, &HarpoonClient::onReconnectTimer);
    connect(&pingTimer, &QTimer::timeout, this, &HarpoonClient::onPingTimer);

    reconnectTimer.setSingleShot(true);
    harpoonUrl = "ws://localhost:8080/ws";
}

HarpoonClient::~HarpoonClient() {
    shutdown = true;
}

void HarpoonClient::run() {
    ws_.open(harpoonUrl);
}

std::list<std::shared_ptr<Server>>& HarpoonClient::getServerListReference() {
    return servers_;
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
    QJsonObject root;
    root["cmd"] = "chat";
    root["server"] = channel->getServer()->getId();
    root["channel"] = channel->getName();
    root["msg"] = message;
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
            irc_handleChatlist(root);
        } else  if (cmd == "chat") {
            irc_handleChat(root);
        } else if (cmd == "join") {
            irc_handleJoin(root);
        }
    }
}

QString HarpoonClient::formatTimestamp(double timestamp) {
    return QTime{QDateTime{QDateTime::fromTime_t(timestamp/1000)}.time()}.toString("[hh:mm:ss]");
}

void HarpoonClient::irc_handleJoin(const QJsonObject& root) {

}

void HarpoonClient::irc_handleChat(const QJsonObject& root) {
    auto timeValue = root.value("time");
    auto nickValue = root.value("nick");
    auto messageValue = root.value("msg");
    auto serverIdValue = root.value("server");
    auto channelNameValue = root.value("channel");
    if (!timeValue.isDouble()) return;
    if (!nickValue.isString()) return;
    if (!messageValue.isString()) return;
    if (!serverIdValue.isString()) return;
    if (!channelNameValue.isString()) return;

    QString time = formatTimestamp(timeValue.toDouble());
    QString nick = nickValue.toString();
    QString message = messageValue.toString();
    QString serverId = serverIdValue.toString();
    QString channelName = channelNameValue.toString();

    auto serverIt = std::find_if(servers_.begin(), servers_.end(), [&serverId](const std::shared_ptr<Server>& server) {
            return server->getId() == serverId;
        });
    if (serverIt == servers_.end()) return;

    Channel* channel = (*serverIt)->getChannel(channelName);
    if (channel == nullptr) return;

    emit beginNewMessage(channel);
    channel->newMessage(time, nick, message);
    emit endNewMessage();
}

void HarpoonClient::irc_handleChatlist(const QJsonObject& root) {
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

        auto currentServer = std::make_shared<Server>(activeNick, serverId, serverName);
        serverList.push_back(currentServer);

        QJsonObject channels = channelsValue.toObject();
        for (auto cit = channels.begin(); cit != channels.end(); ++cit) {
            QString channelName = cit.key();
            QJsonValueRef channelValue = cit.value();
            if (!channelValue.isObject()) return;

            auto currentChannel = std::make_shared<Channel>(currentServer.get(), channelName);
            currentServer->addChannel(currentChannel);

            QJsonObject channel = channelValue.toObject();
            QJsonValue usersValue = channel.value("users");
            if (!usersValue.isObject()) return;

            QJsonObject users = usersValue.toObject();
            for (auto uit = users.begin(); uit != users.end(); ++uit) {
                QString nick = uit.key();
                // TODO: continue
            }
        }
    }

    emit resetServers(serverList);
}
