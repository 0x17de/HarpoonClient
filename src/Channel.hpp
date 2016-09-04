#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>

#include "TreeEntry.hpp"
#include "backlog/BacklogModel.hpp"


class Server;
class Channel : public TreeEntry {
    Server* server_;
    QString name_;
    BacklogModel backlogModel_;
public:
    Channel(Server* server, const QString& name);

    void newMessage(const QString& nick,
                    const QString& message);

    Server* getServer() const;
    QString getName() const;
    BacklogModel* getBacklogModel();
};


#endif
