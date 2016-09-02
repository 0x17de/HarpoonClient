#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include <list>
#include <memory>
#include "channellist/ChannelTreeModel.hpp"


class Server;
class HarpoonClient;
class ChatUi : public QMainWindow {
    Q_OBJECT

    ChannelTreeModel channelTreeModel;

public:
    ChatUi(HarpoonClient& client);

public Q_SLOTS:
    void newServers(const std::list<std::shared_ptr<Server>>& servers);
    void newChannel(const QString& serverId, const QString& name);
};


#endif
