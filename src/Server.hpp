#ifndef SERVER_H
#define SERVER_H


#include <memory>
#include <list>
#include <QString>

#include "TreeEntry.hpp"
#include "models/ChannelTreeModel.hpp"


class Channel;
class Server : public TreeEntry {
    Q_OBJECT

    ChannelTreeModel channelModel_;
    QString id_;
    QString name_;
    QString nick_;
    bool disabled_;

public:
    Server(const QString& activeNick,
           const QString& id,
           const QString& name,
           bool disabled);

    ChannelTreeModel& getChannelModel();
    QString getId() const;
    QString getName() const;
    QString getActiveNick() const;
    void setActiveNick(const QString& nick);
};


#endif
