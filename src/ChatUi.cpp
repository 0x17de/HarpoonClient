#include "ChatUi.hpp"

#include <QTreeWidget>
#include <QScrollBar>
#include "ui_client.h"
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "Channel.hpp"


ChatUi::ChatUi(HarpoonClient& client)
    : channelTreeModel(client.getServerListReference())
{
    Ui::Client{}.setupUi(this);
    connect(&client, &HarpoonClient::resetServers, this, &ChatUi::resetServers);
    connect(&client, &HarpoonClient::resetServers, &channelTreeModel, &ChannelTreeModel::resetServers);
    connect(&client, &HarpoonClient::beginNewMessage, this, &ChatUi::beginNewMessage);
    connect(&client, &HarpoonClient::endNewMessage, this, &ChatUi::endNewMessage);

    channelView = findChild<QTreeView*>("channels");
    channelView->setModel(&channelTreeModel);

    backlogView = findChild<QTableView*>("chat");

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}

ChatUi::~ChatUi() {
    hide();
    channelView->setModel(0);
    backlogView->setModel(0);
}

void ChatUi::resetServers(std::list<std::shared_ptr<Server>>& servers) {
    for (auto& server : servers) {
        auto* channel = server->getChannel(0);
        if (channel != nullptr) {
            activeChannel = channel;
            backlogView->setModel(channel->getBacklogModel());
            backlogView->resizeColumnsToContents();
            backlogView->resizeRowsToContents();
            return;
        }
    }
    activeChannel = 0;
    backlogView->setModel(0);
}

void ChatUi::beginNewMessage(Channel* channel) {
    QScrollBar* bar = backlogView->verticalScrollBar();
    backlogScrollToBottom = activeChannel
        && (bar == nullptr
            || bar->sliderPosition() == bar->maximum());
}

void ChatUi::endNewMessage() {
    if (backlogScrollToBottom)
        backlogView->scrollToBottom();
}
