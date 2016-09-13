#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>
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
public:
    Channel(Server* server, const QString& name);

    void newMessage(const QString& time,
                    const QString& nick,
                    const QString& message);

    Server* getServer() const;
    QString getName() const;
    BacklogModel* getBacklogModel();

signals:
    void resetUsers(std::list<std::shared_ptr<User>> users);
    void beginAddUser(User* user);
    void endAddUser();
};


#endif
