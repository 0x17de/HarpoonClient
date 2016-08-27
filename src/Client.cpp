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
    qDebug() << cmdValue.toString();
}
