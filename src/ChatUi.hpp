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
class QLineEdit;
class QStackedWidget;

class ChatUi : public QMainWindow {
    Q_OBJECT

    ChannelTreeModel channelTreeModel;
    QTreeView* channelView;
    QStackedWidget* userViews;
    QTableView* backlogView;
    QLineEdit* messageInputView;
    Channel* activeChannel;

    bool backlogScrollToBottom;

public:
    ChatUi(HarpoonClient& client);
    ~ChatUi();

private:
    void activateChannel(Channel* channel);

signals:
    void sendMessage(Channel* channel, const QString& message);

public Q_SLOTS:
    void onChannelViewSelection(const QModelIndex& index);
    void expandServer(const QModelIndex& index);
    void channelConnected(Channel* channel);
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void beginNewMessage(Channel* channel);
    void endNewMessage();
    void messageReturnPressed();
};


#endif
