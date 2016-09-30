#include "ChatUi.hpp"

#include <QTreeWidget>
#include <QScrollBar>
#include <QStackedWidget>
#include "ui_client.h"
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"


ChatUi::ChatUi(HarpoonClient& client)
    : client{client}
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

    connect(&client, &HarpoonClient::nickChange, [this](const QString& serverId,
                                                        const QString& timestamp,
                                                        const QString& nick,
                                                        const QString& newNick) {
                Server* server = channelTreeModel.getServer(serverId);
                if (server == nullptr) return;

                if (server->getActiveNick() == nick)
                    server->setActiveNick(newNick);

                for (auto& channel : server->getChannels()) {
                    if (channel->getUserTreeModel()->renameUser(User::stripNick(nick), newNick))
                        channel->getBacklogModel()->addMessage(timestamp, "<->", User::stripNick(nick) + " is now known as " + newNick);
                }
            });

    connect(&client, &HarpoonClient::resetUsers, [this](const QString& serverId,
                                                        const QString& channelName,
                                                        std::list<std::shared_ptr<User>>& users) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getUserTreeModel()->resetUsers(users);
            });

    connect(&client, &HarpoonClient::chatMessage, [this](const QString& serverId,
                                                         const QString& channelName,
                                                         const QString& timestamp,
                                                         const QString& nick,
                                                         const QString& message,
                                                         bool notice) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getBacklogModel()->addMessage(timestamp, notice ? '['+nick+']' : nick, message);
            });

    connect(&client, &HarpoonClient::chatAction, [this](const QString& serverId,
                                                        const QString& channelName,
                                                        const QString& timestamp,
                                                        const QString& nick,
                                                        const QString& message) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getBacklogModel()->addMessage(timestamp, "*", nick + " " + message);
            });

    connect(&client, &HarpoonClient::joinChannel, [this](const QString& serverId,
                                                         const QString& channelName,
                                                         const QString& timestamp,
                                                         const QString& nick) {
                Server* server = channelTreeModel.getServer(serverId);
                if (!server) return;
                if (server->getActiveNick() == User::stripNick(nick)) { // this user
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) // does not exist yet
                        server->addChannel(std::make_shared<Channel>(server, channelName, false));
                    else // re-activate
                        channel->setDisabled(false);
                } else {
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) return;
                    channel->getUserTreeModel()->addUser(std::make_shared<User>(nick));
                    channel->getBacklogModel()->addMessage(timestamp, "-->", nick + " joined the channel");
                }
            });

    connect(&client, &HarpoonClient::partChannel, [this](const QString& serverId,
                                                         const QString& channelName,
                                                         const QString& timestamp,
                                                         const QString& nick) {
                Server* server = channelTreeModel.getServer(serverId);
                if (!server) return;
                if (server->getActiveNick() == User::stripNick(nick)) { // this user
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) // does not exist yet
                        server->addChannel(std::make_shared<Channel>(server, channelName, true));
                    else // disable
                        channel->setDisabled(true);
                } else {
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) return;
                    channel->getUserTreeModel()->removeUser(User::stripNick(nick));
                    channel->getBacklogModel()->addMessage(timestamp, "<--", nick + " left the channel");
                }
            });

    connect(&client, &HarpoonClient::userKicked, [this](const QString& serverId,
                                                        const QString& channelName,
                                                        const QString& timestamp,
                                                        const QString& nick) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getUserTreeModel()->removeUser(User::stripNick(nick));
                channel->getBacklogModel()->addMessage(timestamp, "<--", nick + " was kicked from the channel");
            });

    connect(&client, &HarpoonClient::quitServer, [this](const QString& serverId,
                                                        const QString& timestamp,
                                                        const QString& nick) {
                for (auto& server : channelTreeModel.getServers()) {
                    for (auto& channel : server->getChannels()) {
                        if (channel->getUserTreeModel()->removeUser(User::stripNick(nick)))
                            channel->getBacklogModel()->addMessage(timestamp, "<--", nick + " has quit");
                    }
                }
            });

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
