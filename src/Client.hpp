#ifndef CLIENT_H
#define CLIENT_H

#include <QWebSocket>
#include <QString>
#include <QJsonDocument>


class Client : public QObject {
    Q_OBJECT

public:
    Client();

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onBinaryMessage(const QByteArray& data);
    void handleCommand(const QJsonDocument& doc);

private:
    QWebSocket ws_;
};

#endif
