#ifndef HARPOONCLIENT_H
#define HARPOONCLIENT_H

#include <QWebSocket>
#include <QString>
#include <QTimer>
#include <QSettings>
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
    QString username;
    QString password;

    QWebSocket ws_;

    QString activeNick;
    QTimer reconnectTimer;
    QTimer pingTimer;
    QSettings settings;

public:
    HarpoonClient();
    ~HarpoonClient();
    void run();
    void reconnect(const QString& username,
                   const QString& password,
                   const QString& host);
    QSettings& getSettings();

private:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onBinaryMessage(const QByteArray& data);
    void handleCommand(const QJsonDocument& doc);

    void irc_handleChatList(const QJsonObject& root);
    void irc_handleUserList(const QJsonObject& root);
    void irc_handleTopic(const QJsonObject& root);
    void irc_handleChat(const QJsonObject& root, bool notice);
    void irc_handleAction(const QJsonObject& root);
    void irc_handleJoin(const QJsonObject& root);
    void irc_handlePart(const QJsonObject& root);
    void irc_handleNickChange(const QJsonObject& root);
    void irc_handleQuit(const QJsonObject& root);
    void irc_handleKick(const QJsonObject& root);
    void irc_handleServerAdded(const QJsonObject& root);
    void irc_handleServerDeleted(const QJsonObject& root);

    void onReconnectTimer();
    void onPingTimer();

public Q_SLOTS:
    void sendMessage(Channel* channel, const QString& message);
    void backlogRequest(Channel* channel);

signals:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void resetUsers(const QString& serverId,
                    const QString& channelName,
                    std::list<std::shared_ptr<User>>& userList);
    void newServer(std::shared_ptr<Server> server);
    void serverDeleted(const QString& serverId);
    void joinChannel(size_t id,
                     const QString& serverId,
                     const QString& channelName,
                     double timestamp,
                     const QString& nick);
    void partChannel(size_t id,
                     const QString& serverId,
                     const QString& channelName,
                     double timestamp,
                     const QString& nick);
    void topicChanged(size_t id,
                      const QString& serverId,
                      const QString& channelName,
                      double timestamp,
                      const QString& nick,
                      const QString& topic);
    void nickChange(size_t id,
                    const QString& serverId,
                    double timestamp,
                    const QString& nick,
                    const QString& newNick);
    void quitServer(size_t id,
                    const QString& serverId,
                    double timestamp,
                    const QString& nick);
    void userKicked(size_t id,
                    const QString& serverId,
                    const QString& channelName,
                    double timestamp,
                    const QString& nick,
                    const QString& target,
                    const QString& reason);
    void chatMessage(size_t id,
                     const QString& serverId,
                     const QString& channelName,
                     double timestamp,
                     const QString& nick,
                     const QString& message,
                     bool notice);
    void chatAction(size_t id,
                    const QString& serverId,
                    const QString& channelName,
                    double timestamp,
                    const QString& nick,
                    const QString& action);
};

#endif
