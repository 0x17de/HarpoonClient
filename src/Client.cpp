#include "Client.hpp"
#include <QtCore/QDebug>

QT_USE_NAMESPACE


Client::Client() {
    connect(&ws_, &QWebSocket::connected, this, &Client::onConnected);
    connect(&ws_, &QWebSocket::disconnected, this, &Client::onDisconnected);
    connect(&ws_, &QWebSocket::textMessageReceived, this, &Client::onMessage);

    ws_.open(QUrl("ws://localhost:8080/ws"));
}

void Client::onConnected() {
    qDebug() << "connected";
}

void Client::onDisconnected() {
    qDebug() << "disconnected";
}

void Client::onMessage(QString message) {
    qDebug() << message;
}
