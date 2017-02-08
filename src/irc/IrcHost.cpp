#include "IrcHost.hpp"
#include "moc_IrcHost.cpp"

IrcHost::IrcHost(const std::weak_ptr<IrcServer>& server,
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

QString IrcHost::getHost() const {
    return host_;
}

int IrcHost::getPort() const {
    return port_;
}

bool IrcHost::getSsl() const {
    return ssl_;
}

bool IrcHost::getIpv6() const {
    return ipv6_;
}

std::weak_ptr<IrcServer> IrcHost::getServer() const {
    return server_;
}
