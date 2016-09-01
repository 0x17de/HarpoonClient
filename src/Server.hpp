#ifndef SERVER_H
#define SERVER_H


#include <memory>
#include <list>
#include <QString>


class Channel;
class Server {
    std::list<std::shared_ptr<Channel>> channels_;
    QString name_;
public:
    Server(const QString& name);

    int getChannelCount() const;
    Channel* getChannel(int position);
    QString getName() const;
};


#endif
