#ifndef HOST_H
#define HOST_H

#include "TreeEntry.hpp"


class Server;
class Host : public TreeEntry {
    Q_OBJECT

    std::weak_ptr<Server> server_;
    QString host_;
    int port_;
    bool ssl_;
    bool ipv6_;

public:
    Host(const std::weak_ptr<Server>& server,
         const QString& host,
         int port,
         bool ssl,
         bool ip6);

    QString getHost() const;
    int getPort() const;
    bool getSsl() const;
    bool getIpv6() const;
    std::weak_ptr<Server> getServer() const;

public Q_SLOTS:
signals:
};


#endif
