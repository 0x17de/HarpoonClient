#ifndef HOSTIRC_H
#define HOSTIRC_H

#include "TreeEntry.hpp"


class IrcServer;
class IrcHost : public TreeEntry {
    Q_OBJECT

    std::weak_ptr<IrcServer> server_;
    QString host_;
    int port_;
    bool ssl_;
    bool ipv6_;

public:
    IrcHost(const std::weak_ptr<IrcServer>& server,
         const QString& host,
         int port,
         bool ssl,
         bool ip6);

    QString getHost() const;
    int getPort() const;
    bool getSsl() const;
    bool getIpv6() const;
    std::weak_ptr<IrcServer> getServer() const;

public Q_SLOTS:
signals:
};


#endif
