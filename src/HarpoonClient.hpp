#ifndef HARPOONCLIENT_H
#define HARPOONCLIENT_H

#include <QWebSocket>
#include <QString>
#include <QTimer>
#include <QSettings>
#include <QUrl>
#include <QHash>
#include <list>
#include <memory>


class QJsonObject;
class QJsonDocument;
class IrcServer;
class IrcServerTreeModel;
class SettingsTypeModel;
class IrcHost;
class IrcChannel;
class IrcUser;


class HarpoonClient : public QObject {
    Q_OBJECT

    bool shutdown_;

    IrcServerTreeModel& serverTreeModel_;
    SettingsTypeModel& settingsTypeModel_;

    QUrl harpoonUrl_;
    QString username_;
    QString password_;

    QWebSocket ws_;

    QString activeNick_;
    QTimer reconnectTimer_;
    QTimer pingTimer_;
    QSettings settings_;

public:
    HarpoonClient(IrcServerTreeModel& serverTreeModel,
                  SettingsTypeModel& settingsTypeModel);
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
    void handleLogin(const QJsonObject& root);

    void irc_handleSettings(const QJsonObject& root);
    void irc_handleChatList(const QJsonObject& root);
    void irc_handleUserList(const QJsonObject& root);
    void irc_handleTopic(const QJsonObject& root);
    void irc_handleChat(const QJsonObject& root, bool notice);
    void irc_handleAction(const QJsonObject& root);
    void irc_handleMode(const QJsonObject& root);
    void irc_handleJoin(const QJsonObject& root);
    void irc_handlePart(const QJsonObject& root);
    void irc_handleNickChange(const QJsonObject& root);
    void irc_handleNickModified(const QJsonObject& root);
    void irc_handleQuit(const QJsonObject& root);
    void irc_handleKick(const QJsonObject& root);
    void irc_handleServerAdded(const QJsonObject& root);
    void irc_handleServerDeleted(const QJsonObject& root);
    void irc_handleHostAdded(const QJsonObject& root);
    void irc_handleHostDeleted(const QJsonObject& root);
    void irc_handleBacklogResponse(const QJsonObject& root);

public Q_SLOTS:
    void onReconnectTimer();
    void onPingTimer();
    void onNewChannel(std::shared_ptr<IrcChannel> channel);
    void sendMessage(IrcServer* server, IrcChannel* channel, const QString& message);
    void backlogRequest(IrcChannel* channel);

signals:
    void topicChanged(IrcChannel* channel, const QString& topic);
};

#endif
