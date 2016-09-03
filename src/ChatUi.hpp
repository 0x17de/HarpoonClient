#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include <list>
#include <memory>
#include "channellist/ChannelTreeModel.hpp"


class Server;
class HarpoonClient;
class QTreeView;
class ChatUi : public QMainWindow {
    ChannelTreeModel channelTreeModel;
    QTreeView* channelView;

public:
    ChatUi(HarpoonClient& client);
};


#endif
