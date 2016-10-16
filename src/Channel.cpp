#include "Channel.hpp"

#include <QStackedWidget>
#include <QGraphicsTextItem>
#include <QTextBlockFormat>
#include <QTextCursor>



Channel::Channel(Server* server,
                 const QString& name,
                 bool disabled)
    : TreeEntry('c')
    , server_{server}
    , name_{name}
    , disabled_{disabled}
    , backlogCanvas_(&backlogScene_)
{
    userTreeView_.setHeaderHidden(true);
    userTreeView_.setModel(&userTreeModel_);
    backlogCanvas_.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    backlogCanvas_.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(&userTreeModel_, &UserTreeModel::expand, this, &Channel::expandUserGroup);
    // TODO: connect on resize event => handle chat view
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

BacklogView* Channel::getBacklogView() {
    return &backlogCanvas_;
}

UserTreeModel* Channel::getUserTreeModel() {
    return &userTreeModel_;
}

QTreeView* Channel::getUserTreeView() {
    return &userTreeView_;
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
