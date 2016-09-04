#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include <list>
#include <memory>
#include "channellist/ChannelTreeModel.hpp"


class Server;
class HarpoonClient;
class QTreeView;
class QTableView;

class ChatUi : public QMainWindow {
    Q_OBJECT

    ChannelTreeModel channelTreeModel;
    QTreeView* channelView;
    QTableView* backlogView;

public:
    ChatUi(HarpoonClient& client);
    ~ChatUi();

public Q_SLOTS:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
};


#endif
