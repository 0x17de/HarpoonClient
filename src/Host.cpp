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

QString Host::getHost() const {
    return host;
}

int Host::getPort() const {
    return port;
}

Server* Host::getServer() const {
    return server;
}
