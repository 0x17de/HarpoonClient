#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>
#include <QTableView>
#include <QTreeView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFont>
#include <list>
#include <memory>

#include "BacklogView.hpp"
#include "ChatLine.hpp"
#include "TreeEntry.hpp"
#include "models/UserTreeModel.hpp"


class Server;
class Channel : public TreeEntry {
    Q_OBJECT

    bool backlogRequested;

    size_t firstId_;
    Server* server_;
    QString name_;
    QString topic_;
    UserTreeModel userTreeModel_;
    bool disabled_;
    QTreeView userTreeView_;
    QGraphicsScene backlogScene_; // TODO: create own class + chat line class
    BacklogView backlogCanvas_;

public:
    Channel(size_t firstId,
            Server* server,
            const QString& name,
            bool disabled);
    virtual ~Channel();

    size_t getFirstId() const;
    Server* getServer() const;
    QString getName() const;
    void setTopic(const QString& topic);
    QString getTopic() const;
    bool getDisabled() const;
    void setDisabled(bool disabled);
    void addUser(std::shared_ptr<User> user);
    void resetUsers(std::list<std::shared_ptr<User>>& users);
    User* getUser(const QString& nick);
    void setTopic(size_t id, double timestamp, const QString& nick, const QString& topic);
    void addMessage(size_t id, double timestamp, const QString& nick, const QString& message, bool notice);
    BacklogView* getBacklogView();
    QTreeView* getUserTreeView();
    UserTreeModel& getUserModel();
    void activate();

public Q_SLOTS:
    void expandUserGroup(const QModelIndex& index);

signals:
    void channelDataChanged(Channel* channel);
    void beginAddUser(User* user);
    void endAddUser();
    void backlogRequest(Channel* channel);
};


#endif
