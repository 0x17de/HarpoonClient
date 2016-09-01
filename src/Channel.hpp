#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>


class Server;
class Channel {
    Server* server_;
    QString name_;
public:
    Channel(const QString& name);

    Server* getServer() const;
    QString getName() const;
};


#endif
