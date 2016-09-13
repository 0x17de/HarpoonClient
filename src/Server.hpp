#ifndef SERVER_H
#define SERVER_H


#include <memory>
#include <list>
#include <QString>

#include "TreeEntry.hpp"


class Channel;
class Server : public TreeEntry {
    Q_OBJECT

    std::list<std::shared_ptr<Channel>> channels_;
    QString id_;
    QString name_;
    QString nick_;

public:
    Server(const QString& activeNick,
           const QString& id,
           const QString& name);

    void addChannel(std::shared_ptr<Channel> channel);
    int getChannelCount() const;
    int getChannelIndex(Channel* channel) const;
    Channel* getChannel(QString channelName);
    Channel* getChannel(int position);
    QString getId() const;
    QString getName() const;

signals:
    void beginAddChannel(Channel* channel);
    void endAddChannel();
};


#endif
