#include "Server.hpp"


Server::Server(const QString& id, const QString& name)
    : TreeEntry('s')
    , id_{id}
    , name_{name}
{
}

void Server::addChannel(std::shared_ptr<Channel> channel) {
    channels_.push_back(channel);
}

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

Channel* Server::getChannel(int position) {
    auto it = channels_.begin();
    std::advance(it, position);
    return (*it).get();
}

QString Server::getName() const {
    return name_;
}
