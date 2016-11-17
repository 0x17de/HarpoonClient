#include "ChatUi.hpp"

#include <QTreeWidget>
#include <QStackedWidget>
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "BacklogView.hpp"
#include "Channel.hpp"
#include "User.hpp"


ChatUi::ChatUi(HarpoonClient& client)
    : client{client}
    , settings{client.getSettings()}
    , settingsDialog{client}
{
    clientUi.setupUi(this);
    bouncerConfigurationDialogUi.setupUi(&bouncerConfigurationDialog);

    // bouncer configuration
    bouncerConfigurationDialogUi.username->setText(settings.value("username", "user").toString());
    bouncerConfigurationDialogUi.password->setText(settings.value("password", "password").toString());
    bouncerConfigurationDialogUi.host->setText(settings.value("host", "ws://localhost:8080/ws").toString());

    // assign views
    channelView = clientUi.channels;
    userViews = clientUi.userViews;
    topicView = clientUi.topic;
    backlogViews = clientUi.chats;
    messageInputView = clientUi.message;

    QSplitter* chatSplitter = clientUi.chatSplitter;

    // network configuration
    connect(clientUi.actionConfigure_Networks, &QAction::triggered, [this] { showConfigureNetworksDialog(); });

    // bouncer configuration dialog handling
    connect(clientUi.actionConfigure_Server, &QAction::triggered, [this] { showConfigureBouncerDialog(); });
    connect(&bouncerConfigurationDialog, &QDialog::accepted, [this] {
            QString username = bouncerConfigurationDialogUi.username->text();
            QString password = bouncerConfigurationDialogUi.password->text();
            QString host = bouncerConfigurationDialogUi.host->text();
            settings.setValue("username", username);
            settings.setValue("password", password);
            settings.setValue("host", host);
            this->client.reconnect(username, password, host);
        });

    // channel list events
    connect(&client, &HarpoonClient::resetServers, this, &ChatUi::resetServers);
    connect(&client, &HarpoonClient::resetServers, &channelTreeModel, &ChannelTreeModel::resetServers);
    connect(&client, &HarpoonClient::newServer, &channelTreeModel, &ChannelTreeModel::newServer);
    connect(&client, &HarpoonClient::deleteServer, &channelTreeModel, &ChannelTreeModel::deleteServer);
    connect(channelView, &QTreeView::clicked, this, &ChatUi::onChannelViewSelection);
    connect(&channelTreeModel, &ChannelTreeModel::expand, this, &ChatUi::expandServer);
    connect(&channelTreeModel, &ChannelTreeModel::channelConnected, this, &ChatUi::channelConnected);

    connect(&client, &HarpoonClient::topicChanged, [this](size_t id,
                                                          const QString& serverId,
                                                          const QString& channelName,
                                                          double timestamp,
                                                          const QString& nick,
                                                          const QString& topic) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                if (channel == activeChannel)
                    topicView->setText(topic);
                channel->setTopic(topic);
                channel->getBacklogView()->addMessage(id, timestamp, "!", User::stripNick(nick) + " changed the topic to: " + topic, MessageColor::Event);
            });

    connect(&client, &HarpoonClient::nickChange, [this](size_t id,
                                                        const QString& serverId,
                                                        double timestamp,
                                                        const QString& nick,
                                                        const QString& newNick) {
                Server* server = channelTreeModel.getServer(serverId);
                if (server == nullptr) return;

                if (server->getActiveNick() == nick)
                    server->setActiveNick(newNick);

                for (auto& channel : server->getChannels()) {
                    if (channel->getUserTreeModel()->renameUser(User::stripNick(nick), newNick))
                        channel->getBacklogView()->addMessage(id, timestamp, "<->", User::stripNick(nick) + " is now known as " + newNick, MessageColor::Event);
                }
            });

    connect(&client, &HarpoonClient::resetUsers, [this](const QString& serverId,
                                                        const QString& channelName,
                                                        std::list<std::shared_ptr<User>>& users) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getUserTreeModel()->resetUsers(users);
            });

    connect(&client, &HarpoonClient::chatMessage, [this](size_t id,
                                                         const QString& serverId,
                                                         const QString& channelName,
                                                         double timestamp,
                                                         const QString& nick,
                                                         const QString& message,
                                                         bool notice) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                auto sNick = User::stripNick(nick);
                channel->getBacklogView()->addMessage(id, timestamp, notice ? '['+sNick+']' : '<'+sNick+'>', message, notice ? MessageColor::Notice : MessageColor::Default);
            });

    connect(&client, &HarpoonClient::chatAction, [this](size_t id,
                                                        const QString& serverId,
                                                        const QString& channelName,
                                                        double timestamp,
                                                        const QString& nick,
                                                        const QString& message) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getBacklogView()->addMessage(id, timestamp, "*", User::stripNick(nick) + " " + message, MessageColor::Action);
            });

    connect(&client, &HarpoonClient::joinChannel, [this](size_t id,
                                                         const QString& serverId,
                                                         const QString& channelName,
                                                         double timestamp,
                                                         const QString& nick) {
                Server* server = channelTreeModel.getServer(serverId);
                if (!server) return;
                if (server->getActiveNick() == User::stripNick(nick)) { // this user
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) // does not exist yet
                        server->addChannel(std::make_shared<Channel>(0 /* TODO */, server, channelName, false));
                    else // re-activate
                        channel->setDisabled(false);
                } else {
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) return;
                    channel->getUserTreeModel()->addUser(std::make_shared<User>(nick));
                    channel->getBacklogView()->addMessage(id, timestamp, "-->", nick + " joined the channel", MessageColor::Event);
                }
            });

    connect(&client, &HarpoonClient::partChannel, [this](size_t id,
                                                         const QString& serverId,
                                                         const QString& channelName,
                                                         double timestamp,
                                                         const QString& nick) {
                Server* server = channelTreeModel.getServer(serverId);
                if (!server) return;
                if (server->getActiveNick() == User::stripNick(nick)) { // this user
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) // does not exist yet
                        server->addChannel(std::make_shared<Channel>(0 /* TODO */, server, channelName, true));
                    else // disable
                        channel->setDisabled(true);
                } else {
                    Channel* channel = channelTreeModel.getChannel(server, channelName);
                    if (channel == nullptr) return;
                    channel->getUserTreeModel()->removeUser(User::stripNick(nick));
                    channel->getBacklogView()->addMessage(id, timestamp, "<--", nick + " left the channel", MessageColor::Event);
                }
            });

    connect(&client, &HarpoonClient::userKicked, [this](size_t id,
                                                        const QString& serverId,
                                                        const QString& channelName,
                                                        double timestamp,
                                                        const QString& nick) {
                Channel* channel = channelTreeModel.getChannel(serverId, channelName);
                if (channel == nullptr) return;
                channel->getUserTreeModel()->removeUser(User::stripNick(nick));
                channel->getBacklogView()->addMessage(id, timestamp, "<--", nick + " was kicked from the channel", MessageColor::Event);
            });

    connect(&client, &HarpoonClient::quitServer, [this](size_t id,
                                                        const QString& serverId,
                                                        double timestamp,
                                                        const QString& nick) {
                for (auto& server : channelTreeModel.getServers()) {
                    for (auto& channel : server->getChannels()) {
                        if (channel->getUserTreeModel()->removeUser(User::stripNick(nick)))
                            channel->getBacklogView()->addMessage(id, timestamp, "<--", nick + " has quit", MessageColor::Event);
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

    QWidget* widget;
    while((widget = userViews->currentWidget())) {
        userViews->removeWidget(widget);
        widget->setParent(0);
    }
    while((widget = backlogViews->currentWidget())) {
        backlogViews->removeWidget(widget);
        widget->setParent(0);
    }

    channelView->setModel(0);
}

void ChatUi::showConfigureBouncerDialog() {
    bouncerConfigurationDialog.show();
}

void ChatUi::showConfigureNetworksDialog() {
    settingsDialog.show();
}

void ChatUi::expandServer(const QModelIndex& index) {
    channelView->setExpanded(index, true);
}

void ChatUi::channelConnected(Channel* channel) {
    connect(channel, &Channel::backlogRequest, &client, &HarpoonClient::backlogRequest);
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
        topicView->setText(channel->getTopic());
        channel->activate();
    } else {
        setWindowTitle("Harpoon");
        activeChannel = 0;
        topicView->setText("");
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

void ChatUi::messageReturnPressed() {
    if (activeChannel != nullptr) {
        emit sendMessage(activeChannel, messageInputView->text());
        messageInputView->clear();
    }
}
