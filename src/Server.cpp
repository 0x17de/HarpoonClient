#include "Server.hpp"
#include "Channel.hpp"


Server::Server(const QString& activeNick,
               const QString& id,
               const QString& name)
    : TreeEntry('s')
    , id_{id}
    , name_{name}
    , nick_{activeNick}
{
}

void Server::addChannel(std::shared_ptr<Channel> channel) {
    emit beginAddChannel(channel.get());
    channels_.push_back(channel);
    emit endAddChannel();
}

auto Server::getChannels() -> decltype((channels_)) {
    return channels_;
};

int Server::getChannelCount() const {
    return channels_.size();
}

int Server::getChannelIndex(Channel* channel) const {
    int rowIndex = 0;
    for (auto c : channels_) {
        if (c.get() == channel)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

Channel* Server::getChannel(QString name) {
    auto it = std::find_if(channels_.begin(), channels_.end(), [&name](const std::shared_ptr<Channel>& channel){
            return channel->getName() == name;
        });
    return (it == channels_.end() ? nullptr : (*it).get());
}

Channel* Server::getChannel(int position) {
    auto it = channels_.begin();
    std::advance(it, position);
    return (*it).get();
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
