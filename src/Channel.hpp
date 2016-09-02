#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>

#include "TreeEntry.hpp"


class Server;
class Channel : public TreeEntry {
    Server* server_;
    QString name_;
public:
    Channel(Server* server, const QString& name);

    Server* getServer() const;
    QString getName() const;
};


#endif
