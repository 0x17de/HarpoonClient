#include "irc/IrcServer.hpp"
#include "moc_IrcServer.cpp"
#include "irc/IrcChannel.hpp"


IrcServer::IrcServer(const QString& activeNick,
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

IrcChannelTreeModel& IrcServer::getChannelModel() {
    return channelModel_;
}

IrcHostTreeModel& IrcServer::getHostModel() {
    return hostModel_;
}

IrcNickModel& IrcServer::getNickModel() {
    return nickModel_;
}

QString IrcServer::getId() const {
    return id_;
}

QString IrcServer::getName() const {
    return name_;
}

QString IrcServer::getActiveNick() const {
    return nick_;
}

void IrcServer::setActiveNick(const QString& nick) {
    nick_ = nick;
}

IrcChannel* IrcServer::getBacklog() {
    if (!backlog_)
        backlog_ = std::make_shared<IrcChannel>(std::static_pointer_cast<IrcServer>(shared_from_this()), "["+name_+"]", false);
    return backlog_.get();
}
