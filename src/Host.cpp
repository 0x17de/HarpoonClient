#include "Host.hpp"

Host::Host(const std::weak_ptr<Server>& server,
           const QString& host,
           int port,
           bool ssl,
           bool ipv6)
    : TreeEntry('h')
    , server_{server}
    , host_{host}
    , port_{port}
    , ssl_{ssl}
    , ipv6_{ipv6}
{
}

QString Host::getHost() const {
    return host_;
}

int Host::getPort() const {
    return port_;
}

bool Host::getSsl() const {
    return ssl_;
}

bool Host::getIpv6() const {
    return ipv6_;
}

std::weak_ptr<Server> Host::getServer() const {
    return server_;
}
