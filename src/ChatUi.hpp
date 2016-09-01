#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include "channellist/ChannelTreeModel.hpp"


class HarpoonClient;
class ChatUi : public QMainWindow {
    Q_OBJECT

    ChannelTreeModel channelTreeModel;

public:
    ChatUi(HarpoonClient& client);

public Q_SLOTS:
    void newServer(const QString& serverId, const QString& name);
    void newChannel(const QString& serverId, const QString& name);
};


#endif
