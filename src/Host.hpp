#ifndef HOST_H
#define HOST_H

#include "TreeEntry.hpp"


class Server;
class Host : public TreeEntry {
    Q_OBJECT

    std::weak_ptr<Server> server;
    QString host;
    int port;

public:
    Host(const std::weak_ptr<Server>& server,
         const QString& host,
         int port);

    QString getHost() const;
    int getPort() const;
    std::weak_ptr<Server> getServer() const;

public Q_SLOTS:
signals:
};


#endif
