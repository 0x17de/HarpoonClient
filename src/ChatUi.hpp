#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include "Client.hpp"


class ChannelListWidget;
class ChatUi : public QMainWindow {
    Q_OBJECT

    ChannelListWidget* channelWidget;

public:
    ChatUi(Client& client);

public Q_SLOTS:
    void newServer(const QString& serverId, const QString& name);
    void newChannel(const QString& serverId, const QString& name);
};


#endif
