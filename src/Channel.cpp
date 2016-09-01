#include "Channel.hpp"


Channel::Channel(const QString& name)
    : name_{name}
{
}

Server* Channel::getServer() const {
    return server_;
}

QString Channel::getName() const {
    return name_;
}
