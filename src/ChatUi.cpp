#include "ChatUi.hpp"

#include <QTreeWidget>
#include <QStackedWidget>
#include "HarpoonClient.hpp"
#include "models/ServerTreeModel.hpp"

#include "Server.hpp"
#include "BacklogView.hpp"
#include "Channel.hpp"
#include "User.hpp"


ChatUi::ChatUi(HarpoonClient& client, ServerTreeModel& serverTreeModel)
    : client_{client}
    , serverTreeModel_{serverTreeModel}
    , settings_{client.getSettings()}
    , settingsDialog_{client, serverTreeModel}
{
    clientUi_.setupUi(this);
    bouncerConfigurationDialogUi_.setupUi(&bouncerConfigurationDialog_);

    // bouncer configuration
    bouncerConfigurationDialogUi_.username->setText(settings_.value("username", "user").toString());
    bouncerConfigurationDialogUi_.password->setText(settings_.value("password", "password").toString());
    bouncerConfigurationDialogUi_.host->setText(settings_.value("host", "ws://localhost:8080/ws").toString());

    // assign views
    channelView_ = clientUi_.channels;
    userViews_ = clientUi_.userViews;
    topicView_ = clientUi_.topic;
    backlogViews_ = clientUi_.chats;
    messageInputView_ = clientUi_.message;

    QSplitter* chatSplitter = clientUi_.chatSplitter;

    // network configuration
    connect(clientUi_.actionConfigure_Networks, &QAction::triggered, [this] { showConfigureNetworksDialog(); });

    // bouncer configuration dialog handling
    connect(clientUi_.actionConfigure_Server, &QAction::triggered, [this] { showConfigureBouncerDialog(); });
    connect(&bouncerConfigurationDialog_, &QDialog::accepted, [this] {
            QString username = bouncerConfigurationDialogUi_.username->text();
            QString password = bouncerConfigurationDialogUi_.password->text();
            QString host = bouncerConfigurationDialogUi_.host->text();
            settings_.setValue("username", username);
            settings_.setValue("password", password);
            settings_.setValue("host", host);
            this->client_.reconnect(username, password, host);
        });

    // channel list events
    //connect(&client, &HarpoonClient::resetServers, this, &ChatUi::resetServers);
    connect(channelView_, &QTreeView::clicked, this, &ChatUi::onChannelViewSelection);
    connect(&serverTreeModel_, &ServerTreeModel::expand, this, &ChatUi::expandServer);
    //connect(&serverTreeModel_, &ServerTreeModel::channelConnected, this, &ChatUi::channelConnected);

    connect(&client, &HarpoonClient::topicChanged, [this](size_t id,
                                                          const QString& serverId,
                                                          const QString& channelName,
                                                          double timestamp,
                                                          const QString& nick,
                                                          const QString& topic) {
                Channel* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
                if (channel == nullptr) return;
                if (channel == activeChannel_)
                    topicView_->setText(topic);
                channel->setTopic(topic);
                channel->getBacklogView()->addMessage(id, timestamp, "!", User::stripNick(nick) + " changed the topic to: " + topic, MessageColor::Event);
            });

    connect(&client, &HarpoonClient::nickChange, [this](size_t id,
                                                        const QString& serverId,
                                                        double timestamp,
                                                        const QString& nick,
                                                        const QString& newNick) {
                Server* server = serverTreeModel_.getServer(serverId);
                if (server == nullptr) return;

                if (server->getActiveNick() == nick)
                    server->setActiveNick(newNick);

                for (auto& channel : server->getChannelModel().getChannels()) {
                    if (channel->getUserTreeModel()->renameUser(User::stripNick(nick), newNick))
                        channel->getBacklogView()->addMessage(id, timestamp, "<->", User::stripNick(nick) + " is now known as " + newNick, MessageColor::Event);
                }
            });

    /*connect(&client, &HarpoonClient::resetUsers, [this](const QString& serverId,
                                                        const QString& channelName,
                                                        std::list<std::shared_ptr<User>>& users) {
                Channel* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
                if (channel == nullptr) return;
                channel->getUserTreeModel()->resetUsers(users);
                });*/

    connect(&client, &HarpoonClient::chatMessage, [this](size_t id,
                                                         const QString& serverId,
                                                         const QString& channelName,
                                                         double timestamp,
                                                         const QString& nick,
                                                         const QString& message,
                                                         bool notice) {
                Channel* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
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
                Channel* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
                if (channel == nullptr) return;
                channel->getBacklogView()->addMessage(id, timestamp, "*", User::stripNick(nick) + " " + message, MessageColor::Action);
            });

    connect(&client, &HarpoonClient::joinChannel, [this](size_t id,
                                                         const QString& serverId,
                                                         const QString& channelName,
                                                         double timestamp,
                                                         const QString& nick) {
                Server* server = serverTreeModel_.getServer(serverId);
                if (!server) return;
                if (server->getActiveNick() == User::stripNick(nick)) { // this user
                    Channel* channel = server->getChannelModel().getChannel(channelName);
                    if (channel == nullptr) // does not exist yet
                        server->getChannelModel().newChannel(std::make_shared<Channel>(0 /* TODO */, server, channelName, false));
                    else // re-activate
                        channel->setDisabled(false);
                } else {
                    Channel* channel = server->getChannelModel().getChannel(channelName);
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
                Server* server = serverTreeModel_.getServer(serverId);
                if (!server) return;
                if (server->getActiveNick() == User::stripNick(nick)) { // this user
                    Channel* channel = server->getChannelModel().getChannel(channelName);
                    if (channel == nullptr) // does not exist yet
                        server->getChannelModel().newChannel(std::make_shared<Channel>(0 /* TODO */, server, channelName, true));
                    else // disable
                        channel->setDisabled(true);
                } else {
                    Channel* channel = server->getChannelModel().getChannel(channelName);
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
                Channel* channel = serverTreeModel_.getServer(serverId)->getChannelModel().getChannel(channelName);
                if (channel == nullptr) return;
                channel->getUserTreeModel()->removeUser(User::stripNick(nick));
                channel->getBacklogView()->addMessage(id, timestamp, "<--", nick + " was kicked from the channel", MessageColor::Event);
            });

    connect(&client, &HarpoonClient::quitServer, [this](size_t id,
                                                        const QString& serverId,
                                                        double timestamp,
                                                        const QString& nick) {
                for (auto& server : serverTreeModel_.getServers()) {
                    for (auto& channel : server->getChannelModel().getChannels()) {
                        if (channel->getUserTreeModel()->removeUser(User::stripNick(nick)))
                            channel->getBacklogView()->addMessage(id, timestamp, "<--", nick + " has quit", MessageColor::Event);
                    }
                }
            });

    // input event
    connect(messageInputView_, &QLineEdit::returnPressed, this, &ChatUi::messageReturnPressed);
    connect(this, &ChatUi::sendMessage, &client, &HarpoonClient::sendMessage);

    // assign models
    channelView_->setModel(&serverTreeModel);

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
    messageInputView_->setFocus();
}

ChatUi::~ChatUi() {
    hide();

    QWidget* widget;
    while((widget = userViews_->currentWidget())) {
        userViews_->removeWidget(widget);
        widget->setParent(0);
    }
    while((widget = backlogViews_->currentWidget())) {
        backlogViews_->removeWidget(widget);
        widget->setParent(0);
    }

    channelView_->setModel(0);
}

void ChatUi::showConfigureBouncerDialog() {
    bouncerConfigurationDialog_.show();
}

void ChatUi::showConfigureNetworksDialog() {
    settingsDialog_.show();
}

void ChatUi::expandServer(const QModelIndex& index) {
    channelView_->setExpanded(index, true);
}

void ChatUi::channelConnected(Channel* channel) {
    connect(channel, &Channel::backlogRequest, &client_, &HarpoonClient::backlogRequest);
    userViews_->addWidget(channel->getUserTreeView());
    backlogViews_->addWidget(channel->getBacklogView());
}

void ChatUi::onChannelViewSelection(const QModelIndex& index) {
    auto* item = static_cast<TreeEntry*>(index.internalPointer());
    if (item->getTreeEntryType() == 'c') { // channel selected
        Channel* channel = static_cast<Channel*>(item);
        activateChannel(channel);
        messageInputView_->setFocus();
    }
}

void ChatUi::activateChannel(Channel* channel) {
    if (channel != nullptr) {
        setWindowTitle(QString("Harpoon - ") + channel->getName());
        activeChannel_ = channel;
        if (channel->getUserTreeView()->parentWidget() != nullptr)
            userViews_->setCurrentWidget(channel->getUserTreeView());
        if (channel->getBacklogView()->parentWidget() != nullptr)
            backlogViews_->setCurrentWidget(channel->getBacklogView());
        topicView_->setText(channel->getTopic());
        channel->activate();
    } else {
        setWindowTitle("Harpoon");
        activeChannel_ = nullptr;
        topicView_->setText("");
    }
}

void ChatUi::resetServers(std::list<std::shared_ptr<Server>>& servers) {
    for (auto& server : servers) {
        auto* channel = server->getChannelModel().getChannel(0);
        if (channel) {
            activateChannel(channel);
            return;
        }
    }
    activateChannel(0);
}

void ChatUi::messageReturnPressed() {
    if (activeChannel_ != nullptr) {
        emit sendMessage(activeChannel_, messageInputView_->text());
        messageInputView_->clear();
    }
}
