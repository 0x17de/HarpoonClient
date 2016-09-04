#ifndef SERVER_H
#define SERVER_H


#include <memory>
#include <list>
#include <QString>

#include "TreeEntry.hpp"


class Channel;
class Server : public TreeEntry {
    std::list<std::shared_ptr<Channel>> channels_;
    QString id_;
    QString name_;
public:
    Server(const QString& id, const QString& name);

    void addChannel(std::shared_ptr<Channel> channel);
    int getChannelCount() const;
    int getChannelIndex(Channel* channel) const;
    Channel* getChannel(QString channelName);
    Channel* getChannel(int position);
    QString getId() const;
    QString getName() const;
};


#endif
