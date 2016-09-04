#ifndef HARPOONCLIENT_H
#define HARPOONCLIENT_H

#include <QWebSocket>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <list>
#include <memory>


class QJsonObject;
class QJsonDocument;
class Server;
class Channel;

class HarpoonClient : public QObject {
    Q_OBJECT

    QWebSocket ws_;
    std::list<std::shared_ptr<Server>> servers_;

    QTimer reconnectTimer;
    QTimer pingTimer;

    QUrl harpoonUrl;

public:
    HarpoonClient();
    void run();
    std::list<std::shared_ptr<Server>>& getServerListReference();

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onBinaryMessage(const QByteArray& data);
    void handleCommand(const QJsonDocument& doc);

    void handleChatlist(const QJsonObject& root);

    void irc_handleChat(const QJsonObject& root);

    void onReconnectTimer();
    void onPingTimer();

signals:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void newServer(std::shared_ptr<Server> server);
    void newMessage(Channel* channel,
                    const QString& nick,
                    const QString& message);
};

#endif
