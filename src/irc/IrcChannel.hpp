#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H


#include <QString>
#include <QTableView>
#include <QTreeView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFont>
#include <list>
#include <memory>

#include "irc/IrcBacklogView.hpp"
#include "irc/IrcChatLine.hpp"
#include "TreeEntry.hpp"
#include "models/irc/IrcUserTreeModel.hpp"


class IrcUser;
class IrcServer;
class IrcChannel : public TreeEntry {
    Q_OBJECT

    bool backlogRequested_;

    size_t firstId_;
    std::weak_ptr<IrcServer> server_;
    QString name_;
    QString topic_;
    IrcUserTreeModel userTreeModel_;
    bool disabled_;
    QTreeView userTreeView_;
    QGraphicsScene backlogScene_; // TODO: create own class + chat line class
    IrcBacklogView backlogCanvas_;

public:
    IrcChannel(const std::weak_ptr<IrcServer>& server,
               const QString& name,
               bool disabled);
    virtual ~IrcChannel();

    void onBacklogResponse(size_t firstId);
    size_t getFirstId() const;
    std::weak_ptr<IrcServer> getServer() const;
    QString getName() const;
    QString getTopic() const;
    bool getDisabled() const;
    void setDisabled(bool disabled);
    void addUser(std::shared_ptr<IrcUser> user);
    void resetUsers(std::list<std::shared_ptr<IrcUser>>& users);
    IrcUser* getUser(const QString& nick);
    void setTopic(size_t id, double timestamp, const QString& nick, const QString& topic);
    void addMessage(size_t id, double timestamp, const QString& nick, const QString& message, MessageColor color);
    IrcBacklogView* getBacklogView();
    QTreeView* getUserTreeView();
    IrcUserTreeModel& getUserModel();
    void activate();

public Q_SLOTS:
    void expandUserGroup(const QModelIndex& index);

signals:
    void channelDataChanged(IrcChannel* channel);
    void beginAddUser(IrcUser* user);
    void endAddUser();
    void backlogRequest(IrcChannel* channel);
};


#endif
