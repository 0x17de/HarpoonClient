#include "Server.hpp"
#include "Channel.hpp"


Server::Server(const QString& activeNick,
               const QString& id,
               const QString& name,
               bool disabled)
    : TreeEntry('s')
    , id_{id}
    , name_{name}
    , nick_{activeNick}
    , disabled_{disabled}
{
}

ChannelTreeModel& Server::getChannelModel() {
    return channelModel_;
}

HostTreeModel& Server::getHostModel() {
    return hostModel_;
}

QString Server::getId() const {
    return id_;
}

QString Server::getName() const {
    return name_;
}

QString Server::getActiveNick() const {
    return nick_;
}

void Server::setActiveNick(const QString& nick) {
    nick_ = nick;
}
