#include "ChatUi.hpp"

#include <QTreeWidget>
#include <QScrollBar>
#include <QStackedWidget>
#include "ui_client.h"
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "Channel.hpp"


ChatUi::ChatUi(HarpoonClient& client)
{
    Ui::Client{}.setupUi(this);

    // assign views
    channelView = findChild<QTreeView*>("channels");
    userViews = findChild<QStackedWidget*>("userViews");
    backlogViews = findChild<QStackedWidget*>("chats");
    messageInputView = findChild<QLineEdit*>("message");

    QSplitter* chatSplitter = findChild<QSplitter*>("chatSplitter");

    // channel list events
    connect(&client, &HarpoonClient::resetServers, this, &ChatUi::resetServers);
    connect(&client, &HarpoonClient::resetServers, &channelTreeModel, &ChannelTreeModel::resetServers);
    connect(channelView, &QTreeView::clicked, this, &ChatUi::onChannelViewSelection);
    connect(&channelTreeModel, &ChannelTreeModel::expand, this, &ChatUi::expandServer);
    connect(&channelTreeModel, &ChannelTreeModel::channelConnected, this, &ChatUi::channelConnected);

    // recv message
    // TODO: recv message

    // input event
    connect(messageInputView, &QLineEdit::returnPressed, this, &ChatUi::messageReturnPressed);
    connect(this, &ChatUi::sendMessage, &client, &HarpoonClient::sendMessage);

    // assign models
    channelView->setModel(&channelTreeModel);

    // run
    show();

    // DPI settings could be retrieved via: width() / widthMM()
    // current width
    int splitterWidth = chatSplitter->width();
    const int sidePanelWidth = 200;

    // resize splitter
    if (splitterWidth > sidePanelWidth * 3) { // enough space to fit everything inside?
        QList<int> sizes = {sidePanelWidth, splitterWidth - 2 * sidePanelWidth, sidePanelWidth};
        chatSplitter->setSizes(sizes);
    }

    // set the focus to the input view
    messageInputView->setFocus();
}

ChatUi::~ChatUi() {
    hide();
    channelView->setModel(0);
}

void ChatUi::expandServer(const QModelIndex& index) {
    channelView->setExpanded(index, true);
}

void ChatUi::channelConnected(Channel* channel) {
    userViews->addWidget(channel->getUserTreeView());
    backlogViews->addWidget(channel->getBacklogView());
}

void ChatUi::onChannelViewSelection(const QModelIndex& index) {
    auto* item = static_cast<TreeEntry*>(index.internalPointer());
    if (item->getTreeEntryType() == 'c') { // channel selected
        Channel* channel = static_cast<Channel*>(item);
        activateChannel(channel);
        messageInputView->setFocus();
    }
}

void ChatUi::activateChannel(Channel* channel) {
    if (channel != nullptr) {
        setWindowTitle(QString("Harpoon - ") + channel->getName());
        activeChannel = channel;
        if (channel->getUserTreeView()->parentWidget() != nullptr)
            userViews->setCurrentWidget(channel->getUserTreeView());
        if (channel->getBacklogView()->parentWidget() != nullptr)
            backlogViews->setCurrentWidget(channel->getBacklogView());
    } else {
        setWindowTitle("Harpoon");
        activeChannel = 0;
    }
}

void ChatUi::resetServers(std::list<std::shared_ptr<Server>>& servers) {
    for (auto& server : servers) {
        auto* channel = server->getChannel(0);
        if (channel) {
            activateChannel(channel);
            return;
        }
    }
    activateChannel(0);
}

void ChatUi::beginNewMessage(Channel* channel) {
    if (activeChannel == nullptr) return;
    QScrollBar* bar = activeChannel->getBacklogView()->verticalScrollBar();
    backlogScrollToBottom = activeChannel
        && (bar == nullptr
            || bar->sliderPosition() == bar->maximum());
}

void ChatUi::endNewMessage() {
    if (activeChannel == nullptr) return;
    if (backlogScrollToBottom)
        activeChannel->getBacklogView()->scrollToBottom();
}

void ChatUi::messageReturnPressed() {
    if (activeChannel != nullptr) {
        emit sendMessage(activeChannel, messageInputView->text());
        messageInputView->clear();
    }
}
