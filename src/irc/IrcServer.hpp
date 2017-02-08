#ifndef SERVERIRC_H
#define SERVERIRC_H


#include <memory>
#include <list>
#include <QString>

#include "TreeEntry.hpp"
#include "models/irc/IrcChannelTreeModel.hpp"
#include "models/irc/IrcHostTreeModel.hpp"
#include "models/irc/IrcNickModel.hpp"


class IrcChannel;
class IrcServer : public TreeEntry {
    Q_OBJECT

    IrcChannelTreeModel channelModel_;
    IrcHostTreeModel hostModel_;
    IrcNickModel nickModel_;
    QString id_;
    QString name_;
    QString nick_;
    bool disabled_;
    std::shared_ptr<IrcChannel> backlog_;

public:
    IrcServer(const QString& activeNick,
           const QString& id,
           const QString& name,
           bool disabled);

    IrcChannelTreeModel& getChannelModel();
    IrcHostTreeModel& getHostModel();
    IrcNickModel& getNickModel();
    QString getId() const;
    QString getName() const;
    QString getActiveNick() const;
    void setActiveNick(const QString& nick);
    IrcChannel* getBacklog();
};


#endif
