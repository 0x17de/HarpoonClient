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

    // assign views
    channelView = findChild<QTreeView*>("channels");
    userView = findChild<QTreeView*>("users");
    backlogView = findChild<QTableView*>("chat");
    messageInputView = findChild<QLineEdit*>("message");

    // channel list events
    connect(&client, &HarpoonClient::resetServers, this, &ChatUi::resetServers);
    connect(&client, &HarpoonClient::resetServers, &channelTreeModel, &ChannelTreeModel::resetServers);
    connect(channelView, &QTreeView::clicked, this, &ChatUi::onChannelViewSelection);

    // recv message
    connect(&client, &HarpoonClient::beginNewMessage, this, &ChatUi::beginNewMessage);
    connect(&client, &HarpoonClient::endNewMessage, this, &ChatUi::endNewMessage);

    // input event
    connect(messageInputView, &QLineEdit::returnPressed, this, &ChatUi::messageReturnPressed);
    connect(this, &ChatUi::sendMessage, &client, &HarpoonClient::sendMessage);

    // assign models
    channelView->setModel(&channelTreeModel);
    userView->setModel(0);

    // run
    show();
    messageInputView->setFocus();
}

ChatUi::~ChatUi() {
    hide();
    channelView->setModel(0);
    backlogView->setModel(0);
}

void ChatUi::onChannelViewSelection(const QModelIndex& index) {
    auto* item = static_cast<TreeEntry*>(index.internalPointer());
    if (item->getTreeEntryType() == 'c') { // channel selected
        Channel* channel = static_cast<Channel*>(item);
        activeChannel = channel;
        backlogView->setModel(channel->getBacklogModel());
        userView->setModel(channel->getUserTreeModel());
    }
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

void ChatUi::messageReturnPressed() {
    if (activeChannel != nullptr) {
        emit sendMessage(activeChannel, messageInputView->text());
        messageInputView->clear();
    }
}
