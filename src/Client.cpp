#include "Client.hpp"
#include <QtCore/QDebug>
#include <QJsonObject>
#include <QJsonValue>

QT_USE_NAMESPACE


Client::Client() {
    connect(&ws_, &QWebSocket::connected, this, &Client::onConnected);
    connect(&ws_, &QWebSocket::disconnected, this, &Client::onDisconnected);
    connect(&ws_, &QWebSocket::textMessageReceived, this, &Client::onTextMessage);
    connect(&ws_, &QWebSocket::binaryMessageReceived, this, &Client::onBinaryMessage);
}

void Client::run() {
    ws_.open(QUrl("ws://localhost:8080/ws"));
}

void Client::onConnected() {
    qDebug() << "connected";
    ws_.sendTextMessage("LOGIN user password\n");
}

void Client::onDisconnected() {
    qDebug() << "disconnected";
}

void Client::onTextMessage(const QString& message) {
    qDebug() << message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    handleCommand(doc);
}

void Client::onBinaryMessage(const QByteArray& data) {
    qDebug() << data;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    handleCommand(doc);
}

void Client::handleCommand(const QJsonDocument& doc) {
    if (!doc.isObject()) return;
    QJsonObject root = doc.object();
    QJsonValue cmdValue = root.value("cmd");
    if (!cmdValue.isString()) return;

    QString cmd = cmdValue.toString();
    qDebug() << cmd;
    if (cmd == "chatlist") {
        QJsonValue serversValue = root.value("servers");
        if (!serversValue.isObject()) return;

        QJsonObject servers = serversValue.toObject();
        for (auto sit = servers.begin(); sit != servers.end(); ++sit) {
            QString serverId = sit.key();
            QJsonValueRef serverValue = sit.value();
            if (!serverValue.isObject()) return;

            QJsonObject server = serverValue.toObject();
            QJsonValue channelsValue = server.value("channels");
            if (!channelsValue.isObject()) return;

            QJsonObject channels = channelsValue.toObject();
            for (auto cit = channels.begin(); cit != channels.end(); ++cit) {
                QString channelName = cit.key();
                QJsonValueRef channelValue = cit.value();
                if (!channelValue.isObject()) return;

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
    }
}
