#include "Channel.hpp"


Channel::Channel(Server* server, const QString& name)
    : TreeEntry('c')
    , server_{server}
    , name_{name}
{
}

Server* Channel::getServer() const {
    return server_;
}

QString Channel::getName() const {
    return name_;
}
