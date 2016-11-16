#ifndef HOST_H
#define HOST_H

#include "TreeEntry.hpp"


class Server;
class Host : public TreeEntry {
    Q_OBJECT

    Server* server;
    QString host;
    int port;

public:
    Host(Server* server,
         const QString& host,
         int port);

    Server* getServer() const;

public Q_SLOTS:
signals:
};


#endif
