#ifndef HARPOONCLIENT_H
#define HARPOONCLIENT_H

#include <QWebSocket>
#include <QString>
#include <QJsonDocument>
#include <list>
#include <memory>


class Server;
class HarpoonClient : public QObject {
    Q_OBJECT

public:
    HarpoonClient();
    void run();

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onBinaryMessage(const QByteArray& data);
    void handleCommand(const QJsonDocument& doc);

signals:
    void newServers(const std::list<std::shared_ptr<Server>>& servers);
    void newChannel(const QString& serverId, const QString& name);

private:
    QWebSocket ws_;
};

#endif
