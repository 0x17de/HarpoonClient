#include "IrcChannel.hpp"
#include "moc_IrcChannel.cpp"
#include "IrcUser.hpp"
#include "IrcServer.hpp"

#include <QStackedWidget>
#include <QGraphicsTextItem>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QScrollBar>


IrcChannel::IrcChannel(size_t firstId,
                 const std::weak_ptr<IrcServer>& server,
                 const QString& name,
                 bool disabled)
    : TreeEntry('c')
    , backlogRequested{false}
    , server_{server}
    , name_{name}
    , disabled_{disabled}
    , backlogCanvas_(&backlogScene_)
{
    userTreeView_.setHeaderHidden(true);
    userTreeView_.setModel(&userTreeModel_);
    backlogCanvas_.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    backlogCanvas_.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(&userTreeModel_, &IrcUserTreeModel::expand, this, &IrcChannel::expandUserGroup);
    // TODO: connect on resize event => handle chat view
}

IrcChannel::~IrcChannel() {
    userTreeView_.setModel(0);
}

void IrcChannel::activate() {
    QScrollBar* bar = backlogCanvas_.verticalScrollBar();
    if (bar && bar->sliderPosition() == 0) {
        if (!backlogRequested) {
            backlogRequested = true;
            emit backlogRequest(this);
        }
    }
}

size_t IrcChannel::getFirstId() const {
    return firstId_;
}

std::weak_ptr<IrcServer> IrcChannel::getServer() const {
    return server_;
}

QString IrcChannel::getName() const {
    return name_;
}

QString IrcChannel::getTopic() const {
    return topic_;
}

bool IrcChannel::getDisabled() const {
    return disabled_;
}

void IrcChannel::setDisabled(bool disabled) {
    if (disabled_ != disabled) {
        disabled_ = disabled;

        std::list<std::shared_ptr<IrcUser>> newUsers;
        userTreeModel_.resetUsers(newUsers);

        if (auto s = server_.lock())
            s->getChannelModel().channelDataChanged(this);
    }
}

void IrcChannel::expandUserGroup(const QModelIndex& index) {
    userTreeView_.setExpanded(index, true);
}

IrcBacklogView* IrcChannel::getBacklogView() {
    return &backlogCanvas_;
}

IrcUserTreeModel& IrcChannel::getUserModel() {
    return userTreeModel_;
}

QTreeView* IrcChannel::getUserTreeView() {
    return &userTreeView_;
}

void IrcChannel::addUser(std::shared_ptr<IrcUser> user) {
    userTreeModel_.addUser(user);
}

void IrcChannel::resetUsers(std::list<std::shared_ptr<IrcUser>>& users) {
    userTreeModel_.resetUsers(users);
}

IrcUser* IrcChannel::getUser(const QString& nick) {
    return userTreeModel_.getUser(nick);
}

void IrcChannel::setTopic(size_t id, double timestamp, const QString& nick, const QString& topic) {
    topic_ = topic;
    backlogCanvas_.addMessage(id, timestamp, "!", IrcUser::stripNick(nick) + " changed the topic to: " + topic, MessageColor::Event);
}

void IrcChannel::addMessage(size_t id, double timestamp, const QString& nick, const QString& message, MessageColor color) {
    backlogCanvas_.addMessage(id, timestamp, nick, message, color);
}
