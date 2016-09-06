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

    bool shutdown;

    std::list<std::shared_ptr<Server>> servers_;
    QUrl harpoonUrl;

    QWebSocket ws_;

    QString activeNick;
    QTimer reconnectTimer;
    QTimer pingTimer;

public:
    HarpoonClient();
    ~HarpoonClient();
    void run();
    std::list<std::shared_ptr<Server>>& getServerListReference();

private:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onBinaryMessage(const QByteArray& data);
    void handleCommand(const QJsonDocument& doc);

    QString formatTimestamp(double timestamp);

    void irc_handleChatlist(const QJsonObject& root);
    void irc_handleChat(const QJsonObject& root);
    void irc_handleJoin(const QJsonObject& root);

    void onReconnectTimer();
    void onPingTimer();

public Q_SLOTS:
    void sendMessage(Channel* channel, const QString& message);

signals:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void newServer(std::shared_ptr<Server> server);
    void beginNewMessage(Channel* channel);
    void endNewMessage();
};

#endif
