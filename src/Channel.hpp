#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>
#include <QTableView>
#include <QTreeView>
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
    BacklogModel backlogModel_;
    UserTreeModel userTreeModel_;
    bool disabled_;
    QTreeView userTreeView_;
    QTableView backlogView_;
public:
    Channel(Server* server,
            const QString& name,
            bool disabled);

    void newMessage(const QString& time,
                    const QString& nick,
                    const QString& message);

    Server* getServer() const;
    QString getName() const;
    bool getDisabled() const;
    void setDisabled(bool disabled);
    void addUser(std::shared_ptr<User> user);
    void resetUsers(std::list<std::shared_ptr<User>>& users);
    User* getUser(QString nick);
    QTableView* getBacklogView();
    BacklogModel* getBacklogModel();
    QTreeView* getUserTreeView();
    UserTreeModel* getUserTreeModel();

signals:
    void channelDataChanged(Channel* channel);
    void beginAddUser(User* user);
    void endAddUser();
};


#endif
