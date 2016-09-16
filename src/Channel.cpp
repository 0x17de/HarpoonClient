#include "Channel.hpp"


Channel::Channel(Server* server,
                 const QString& name,
                 bool disabled)
    : TreeEntry('c')
    , server_{server}
    , name_{name}
    , disabled_{disabled}
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

bool Channel::getDisabled() const {
    return disabled_;
}

void Channel::setDisabled(bool disabled) {
    if (disabled_ != disabled) {
        disabled_ = disabled;
        emit channelDataChanged(this);
    }
}

UserTreeModel* Channel::getUserTreeModel() {
    return &userTreeModel_;
}

BacklogModel* Channel::getBacklogModel() {
    return &backlogModel_;
}
