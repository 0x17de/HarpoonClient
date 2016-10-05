#include "Channel.hpp"

#include <QStackedWidget>
#include <QGraphicsTextItem>


Channel::Channel(Server* server,
                 const QString& name,
                 bool disabled)
    : TreeEntry('c')
    , server_{server}
    , name_{name}
    , disabled_{disabled}
    , backlogCanvas_{&backlogScene_}
{
    backlogView_.setModel(&backlogModel_);
    userTreeView_.setModel(&userTreeModel_);
    backlogCanvas_.setAlignment(Qt::AlignLeft | Qt::AlignTop);

    connect(&userTreeModel_, &UserTreeModel::expand, this, &Channel::expandUserGroup);

    // TODO: for debugging reasons, later in scene
    static size_t row = 0;
    connect(&backlogModel_, &BacklogModel::rowsInserted, [this](const QModelIndex& index,
                                                                int rowStart,
                                                                int rowEnd) {
                for (size_t i = rowStart; i <= rowEnd; ++i) {
                    auto time = backlogModel_.data(backlogModel_.index(i, 0, index), Qt::DisplayRole);
                    backlogScene_.addText(time.toString())->setPos(0, row * 20);
                    auto nick = backlogModel_.data(backlogModel_.index(i, 1, index), Qt::DisplayRole);
                    backlogScene_.addText(nick.toString())->setPos(80, row * 20);
                    auto message = backlogModel_.data(backlogModel_.index(i, 2, index), Qt::DisplayRole);
                    backlogScene_.addText(message.toString())->setPos(200, row * 20);
                    ++row;
                }
        });
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

QGraphicsView* Channel::getBacklogView() {
    return &backlogCanvas_;
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
