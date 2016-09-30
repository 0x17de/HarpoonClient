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
class User;


class HarpoonClient : public QObject {
    Q_OBJECT

    bool shutdown;

    QUrl harpoonUrl;

    QWebSocket ws_;

    QString activeNick;
    QTimer reconnectTimer;
    QTimer pingTimer;

public:
    HarpoonClient();
    ~HarpoonClient();
    void run();

private:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onBinaryMessage(const QByteArray& data);
    void handleCommand(const QJsonDocument& doc);

    QString formatTimestamp(double timestamp);

    void irc_handleChatList(const QJsonObject& root);
    void irc_handleUserList(const QJsonObject& root);
    void irc_handleChat(const QJsonObject& root);
    void irc_handleNotice(const QJsonObject& root);
    void irc_handleAction(const QJsonObject& root);
    void irc_handleJoin(const QJsonObject& root);
    void irc_handlePart(const QJsonObject& root);
    void irc_handleNickChange(const QJsonObject& root);
    void irc_handleQuit(const QJsonObject& root);
    void irc_handleKick(const QJsonObject& root);

    void onReconnectTimer();
    void onPingTimer();

public Q_SLOTS:
    void sendMessage(Channel* channel, const QString& message);

signals:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void resetUsers(const QString& serverId,
                    const QString& channelName,
                    std::list<std::shared_ptr<User>>& userList);
    void newServer(std::shared_ptr<Server> server);
    void joinChannel(const QString& serverId,
                     const QString& channelName,
                     const QString& timestamp,
                     const QString& nick);
    void partChannel(const QString& serverId,
                     const QString& channelName,
                     const QString& timestamp,
                     const QString& nick);
    void nickChange(const QString& serverId,
                    const QString& timestamp,
                    const QString& nick,
                    const QString& newNick);
    void quitServer(const QString& serverId,
                    const QString& timestamp,
                    const QString& nick);
    void userKicked(const QString& serverId,
                    const QString& channelName,
                    const QString& timestamp,
                    const QString& nick,
                    const QString& target,
                    const QString& reason);
    void chatMessage(const QString& serverId,
                     const QString& channelName,
                     const QString& timestamp,
                     const QString& nick,
                     const QString& message,
                     bool notice);
    void chatAction(const QString& serverId,
                    const QString& channelName,
                    const QString& timestamp,
                    const QString& nick,
                    const QString& action);
};

#endif
