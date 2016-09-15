#include "Channel.hpp"


Channel::Channel(Server* server, const QString& name)
    : TreeEntry('c')
    , server_{server}
    , name_{name}
{
}

void Channel::newMessage(const QString& time,
                         const QString& nick,
                         const QString& message) {
    backlogModel_.addMessage(time, nick, message);
}

Server* Channel::getServer() const {
    return server_;
}

QString Channel::getName() const {
    return name_;
}

UserTreeModel* Channel::getUserTreeModel() {
    return &userTreeModel_;
}

BacklogModel* Channel::getBacklogModel() {
    return &backlogModel_;
}
