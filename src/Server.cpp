#include "Server.hpp"


Server::Server(const QString& name)
    : name_{name}
{
}

int Server::getChannelCount() const {
    return channels_.size();
}

Channel* Server::getChannel(int position) {
    auto it = channels_.begin();
    std::advance(it, position);
    return (*it).get();
}

QString Server::getName() const {
    return name_;
}
