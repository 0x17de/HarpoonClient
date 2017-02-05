#include "ChatUi.hpp"
#include "moc_ChatUi.cpp"

#include <functional>
#include <QCoreApplication>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QStringListModel>
#include <QDesktopServices>
#include "HarpoonClient.hpp"
#include "models/ServerTreeModel.hpp"
#include "version.hpp"

#include "Server.hpp"
#include "BacklogView.hpp"
#include "Channel.hpp"
#include "User.hpp"


ChatUi::ChatUi(HarpoonClient& client,
               ServerTreeModel& serverTreeModel,
               SettingsTypeModel& settingsTypeModel)
    : settings_{client.getSettings()}
    , client_{client}
    , serverTreeModel_{serverTreeModel}
    , settingsTypeModel_{settingsTypeModel}
    , settingsDialog_{client, serverTreeModel, settingsTypeModel}
{
    clientUi_.setupUi(this);
    bouncerConfigurationDialogUi_.setupUi(&bouncerConfigurationDialog_);

    using UrlMap = QMap<QString, QUrl>;
    UrlMap aboutUrls{
        {"JsonCpp", QUrl("https://github.com/open-source-parsers/jsoncpp")},
        {"libircclient", QUrl("https://www.ulduzsoft.com/libircclient/")},
        {"Qt5", QUrl("https://www.qt.io/")},
        {"SOCI", QUrl("http://soci.sourceforge.net/")},
        {"OpenSSL", QUrl("https://www.openssl.org/")},
    };
    QStringList usedLibraries{
        "JsonCpp",
        "libircclient",
        "Qt5",
        "SOCI", 
        "OpenSSL"
    };
    aboutDialogUi_.setupUi(&aboutDialog_);
    aboutDialogUi_.clientVersion->setText(QString("HarpoonClient v")+QString::number(VERSION_MAJOR)+"."+QString::number(VERSION_MINOR)+"."+QString::number(VERSION_PATCH));
    aboutDialogUi_.libraryList->setEditTriggers(QAbstractItemView::NoEditTriggers); // can't edit the text items
    aboutDialogUi_.libraryList->setModel(new QStringListModel(usedLibraries, aboutDialogUi_.libraryList));
    connect(clientUi_.actionAbout, &QAction::triggered, [this]{ aboutDialog_.show(); });
    connect(aboutDialogUi_.libraryList, &QAbstractItemView::doubleClicked, std::bind([this](const UrlMap& aboutUrls, const QModelIndex& index){
            QString item = index.data(Qt::DisplayRole).toString();
            auto it = aboutUrls.find(item);
            if (it != aboutUrls.end())
                QDesktopServices::openUrl(it.value());
            }, std::move(aboutUrls), std::placeholders::_1));

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

    // quit action
    connect(clientUi_.actionQuit, &QAction::triggered, []{
            QCoreApplication::quit();
        });

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
    connect(channelView_, &QTreeView::clicked, this, &ChatUi::onChannelViewSelection);
    connect(&serverTreeModel_, &ServerTreeModel::expand, this, &ChatUi::expandServer);

    connect(&client, &HarpoonClient::topicChanged, [this](Channel* channel, const QString& topic) {
            if (activeChannel_ == channel)
                topicView_->setText(topic);
        });

    channelView_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(channelView_, &QWidget::customContextMenuRequested, this, &ChatUi::showChannelContextMenu);

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

void ChatUi::showChannelContextMenu(const QPoint&) {
    auto index = channelView_->selectionModel()->currentIndex();
    auto* item = static_cast<TreeEntry*>(index.internalPointer());

    std::shared_ptr<Server> server;
    std::shared_ptr<Channel> channel;

    if (item->getTreeEntryType() == 's') {
        server = std::static_pointer_cast<Server>(item->shared_from_this());
    } else if (item->getTreeEntryType() == 'c') {
        channel = std::static_pointer_cast<Channel>(item->shared_from_this());
        server = channel->getServer().lock();
    }

    if (channel == nullptr) return;

    QMenu menu(this);

    QAction *actionJoin = nullptr, *actionPart = nullptr, *actionDelete = nullptr;

    if (channel->getDisabled()) {
        actionJoin = menu.addAction("Join");
    } else {
        actionPart = menu.addAction("Part");
    }
    actionDelete = menu.addAction("Delete");

    QAction* selected = menu.exec(QCursor::pos());
    if (selected == nullptr) return;

    if (selected == actionJoin) {
        client_.sendMessage(server.get(), channel.get(), "/join "+channel->getName());
    } else if (selected == actionPart) {
        client_.sendMessage(server.get(), channel.get(), "/part "+channel->getName());
    } else if (selected == actionDelete) {
        client_.sendMessage(server.get(), channel.get(), "/deletechannel "+channel->getName());
    }
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

void ChatUi::onChannelViewSelection(const QModelIndex& index) {
    auto* item = static_cast<TreeEntry*>(index.internalPointer());
    auto type = item->getTreeEntryType();
    if (type == 'c') { // channel selected
        Channel* channel = static_cast<Channel*>(item);
        activateChannel(channel);
        messageInputView_->setFocus();
    } else if (type == 's') {
        Server* server = static_cast<Server*>(item);
        activateChannel(server->getBacklog());
        messageInputView_->setFocus();
    }
}

void ChatUi::activateChannel(Channel* channel) {
    if (channel != nullptr) {
        setWindowTitle(QString("Harpoon - ") + channel->getName());
        activeChannel_ = channel;
        if (channel->getUserTreeView()->parentWidget() == nullptr)
            userViews_->addWidget(channel->getUserTreeView());
        userViews_->setCurrentWidget(channel->getUserTreeView());
        if (channel->getBacklogView()->parentWidget() == nullptr)
            backlogViews_->addWidget(channel->getBacklogView());
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
        auto server = activeChannel_->getServer().lock();
        if (server) {
            emit sendMessage(server.get(), activeChannel_, messageInputView_->text());
            messageInputView_->clear();
        }
    }
}
