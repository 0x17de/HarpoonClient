#include "Host.hpp"

Host::Host(Server* server,
           const QString& host,
           int port)
    : TreeEntry('h')
    , server{server}
    , host{host}
    , port{port}
{
}

Server* Host::getServer() const {
    return server;
}
