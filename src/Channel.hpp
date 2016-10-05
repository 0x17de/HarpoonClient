#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>
#include <QTableView>
#include <QTreeView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <list>
#include <memory>

#include "TreeEntry.hpp"
#include "backlog/BacklogModel.hpp"
#include "userlist/UserTreeModel.hpp"


class Server;
class Channel : public TreeEntry {
    Q_OBJECT

    Server* server_;
    QString name_;
    QString topic_;
    BacklogModel backlogModel_;
    UserTreeModel userTreeModel_;
    bool disabled_;
    QTreeView userTreeView_;
    QTableView backlogView_; // TODO: completely replace with graphicsview
    QGraphicsScene backlogScene_; // TODO: create own class + chat line class
    QGraphicsView backlogCanvas_;

public:
    Channel(Server* server,
            const QString& name,
            bool disabled);

    Server* getServer() const;
    QString getName() const;
    void setTopic(const QString& topic);
    QString getTopic() const;
    bool getDisabled() const;
    void setDisabled(bool disabled);
    void addUser(std::shared_ptr<User> user);
    void resetUsers(std::list<std::shared_ptr<User>>& users);
    User* getUser(QString nick);
    QGraphicsView* getBacklogView();
    BacklogModel* getBacklogModel();
    QTreeView* getUserTreeView();
    UserTreeModel* getUserTreeModel();

public Q_SLOTS:
    void expandUserGroup(const QModelIndex& index);

signals:
    void channelDataChanged(Channel* channel);
    void beginAddUser(User* user);
    void endAddUser();
};


#endif
