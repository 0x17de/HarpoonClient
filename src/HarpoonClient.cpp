#include "HarpoonClient.hpp"
#include <QtCore/QDebug>
#include <QJsonObject>
#include <QJsonValue>

#include "Server.hpp"
#include "Channel.hpp"

QT_USE_NAMESPACE


HarpoonClient::HarpoonClient() {
    connect(&ws_, &QWebSocket::connected, this, &HarpoonClient::onConnected);
    connect(&ws_, &QWebSocket::disconnected, this, &HarpoonClient::onDisconnected);
    connect(&ws_, &QWebSocket::textMessageReceived, this, &HarpoonClient::onTextMessage);
    connect(&ws_, &QWebSocket::binaryMessageReceived, this, &HarpoonClient::onBinaryMessage);
}

void HarpoonClient::run() {
    ws_.open(QUrl("ws://localhost:8080/ws"));
}

void HarpoonClient::onConnected() {
    qDebug() << "connected";
    ws_.sendTextMessage("LOGIN user password\n");
}

void HarpoonClient::onDisconnected() {
    qDebug() << "disconnected";
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

void HarpoonClient::handleCommand(const QJsonDocument& doc) {
    if (!doc.isObject()) return;
    QJsonObject root = doc.object();
    QJsonValue cmdValue = root.value("cmd");
    if (!cmdValue.isString()) return;

    QString cmd = cmdValue.toString();
    qDebug() << cmd;
    if (cmd == "chatlist") {
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
            QJsonValue channelsValue = server.value("channels");
            if (!channelsValue.isObject()) return;

            auto currentServer = std::make_shared<Server>(serverId, serverName);
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

        emit newServers(serverList);
    }
}
