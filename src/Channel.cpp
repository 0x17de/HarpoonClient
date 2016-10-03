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
    connect(&userTreeModel_, &UserTreeModel::expand, this, &Channel::expandUserGroup);
}

Server* Channel::getServer() const {
    return server_;
}

QString Channel::getName() const {
    return name_;
}

void Channel::setTopic(const QString& topic) {
    topic_ = topic;
}

QString Channel::getTopic() const {
    return topic_;
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

void Channel::expandUserGroup(const QModelIndex& index) {
    userTreeView_.setExpanded(index, true);
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
