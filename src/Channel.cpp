#include "Channel.hpp"

#include <QStackedWidget>


Channel::Channel(Server* server,
                 const QString& name,
                 bool disabled)
    : TreeEntry('c')
    , server_{server}
    , name_{name}
    , disabled_{disabled}
{
    backlogView_.setModel(&backlogModel_);
    userTreeView_.setModel(&userTreeModel_);
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

bool Channel::getDisabled() const {
    return disabled_;
}

void Channel::setDisabled(bool disabled) {
    if (disabled_ != disabled) {
        disabled_ = disabled;
        emit channelDataChanged(this);
    }
}

QTableView* Channel::getBacklogView() {
    return &backlogView_;
}

UserTreeModel* Channel::getUserTreeModel() {
    return &userTreeModel_;
}

QTreeView* Channel::getUserTreeView() {
    return &userTreeView_;
}

BacklogModel* Channel::getBacklogModel() {
    return &backlogModel_;
}

void Channel::addUser(std::shared_ptr<User> user) {
    userTreeModel_.addUser(user);
}

void Channel::resetUsers(std::list<std::shared_ptr<User>>& users) {
    userTreeModel_.resetUsers(users);
}

User* Channel::getUser(QString nick) {
    return userTreeModel_.getUser(nick);
}
