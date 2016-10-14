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
    , splitting_{0.15, 0.15, 0.7}
{
    userTreeView_.setHeaderHidden(true);
    userTreeView_.setModel(&userTreeModel_);
    backlogCanvas_.setAlignment(Qt::AlignLeft | Qt::AlignTop);

    connect(&userTreeModel_, &UserTreeModel::expand, this, &Channel::expandUserGroup);
    // TODO: connect on resize event => handle chat view
}

void Channel::resizeLines() {
    auto contentsRect = backlogCanvas_.contentsRect();
    qreal width = contentsRect.width();
    qreal timeWidth = splitting_[0] * width;
    qreal whoWidth = splitting_[1] * width;
    qreal messageWidth = splitting_[2] * width;

    int top = 0;
    int left = 0;
    for (auto& line : chatLines_) {
        left = 0;
        auto* timeGfx = line.getTimeGfx();
        auto* whoGfx = line.getWhoGfx();
        auto* messageGfx = line.getMessageGfx();
        timeGfx->setTextWidth(timeWidth);
        whoGfx->setTextWidth(whoWidth);
        messageGfx->setTextWidth(messageWidth);

        timeGfx->setPos(left, top);
        left += timeWidth;
        whoGfx->setPos(left, top);
        left += whoWidth;
        messageGfx->setPos(left, top);

        top += std::max({timeGfx->boundingRect().height(), whoGfx->boundingRect().height(), messageGfx->boundingRect().height()});
    }
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

void Channel::addMessage(const QString& time,
                         const QString& nick,
                         const QString& message,
                         const MessageColor color) {
    chatLines_.emplace_back(time, nick, message, color);
    ChatLine& line = chatLines_.back();
    backlogScene_.addItem(line.getTimeGfx());
    backlogScene_.addItem(line.getWhoGfx());
    backlogScene_.addItem(line.getMessageGfx());

    resizeLines();
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

void Channel::addUser(std::shared_ptr<User> user) {
    userTreeModel_.addUser(user);
}

void Channel::resetUsers(std::list<std::shared_ptr<User>>& users) {
    userTreeModel_.resetUsers(users);
}

User* Channel::getUser(QString nick) {
    return userTreeModel_.getUser(nick);
}
