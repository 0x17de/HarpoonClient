#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include <list>
#include <memory>
#include "channellist/ChannelTreeModel.hpp"


class Server;
class Channel;
class HarpoonClient;
class QTreeView;
class QTableView;

class ChatUi : public QMainWindow {
    Q_OBJECT

    ChannelTreeModel channelTreeModel;
    QTreeView* channelView;
    QTableView* backlogView;
    Channel* activeChannel;

    bool backlogScrollToBottom;

public:
    ChatUi(HarpoonClient& client);
    ~ChatUi();

public Q_SLOTS:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void beginNewMessage(Channel* channel);
    void endNewMessage();
};


#endif
