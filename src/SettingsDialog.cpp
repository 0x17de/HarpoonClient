#include "SettingsDialog.hpp"
#include "models/ServerTreeModel.hpp"
#include "models/SettingsTypeModel.hpp"
#include "HarpoonClient.hpp"
#include "Host.hpp"


SettingsDialog::SettingsDialog(HarpoonClient& client,
                               ServerTreeModel& channelTreeModel,
                               SettingsTypeModel& settingsTypeModel)
    : client_{client}
    , channelTreeModel_{channelTreeModel}
    , settingsTypeModel_{settingsTypeModel}
{
    // settings dialog
    settingsDialogUi_.setupUi(&settingsDialog_);
    ircSettingsUi_.setupUi(&ircSettingsWidget_);

    widgetMap_.insert("irc", &ircSettingsWidget_);
    settingsDialogUi_.protocolSelection->setModel(&settingsTypeModel);
    settingsDialogUi_.protocolSettings->addWidget(&ircSettingsWidget_);
    ircSettingsUi_.serverList->setModel(&channelTreeModel);

    connect(settingsDialogUi_.protocolSelection, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::activated), this, &SettingsDialog::onProtocolSelected);
    connect(ircSettingsUi_.serverList, &QListView::clicked, this, &SettingsDialog::onIrcServerSelected);
    connect(ircSettingsUi_.btnReconnect, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            QString serverId = getSelectedServer()->getId();
            client_.sendMessage(server.get(), nullptr, "/reconnect");
        });
}

SettingsDialog::~SettingsDialog() {
    settingsDialog_.hide();
    ircSettingsUi_.serverList->setModel(0);
    ircSettingsUi_.hostsList->setModel(0);
    settingsDialogUi_.protocolSettings->removeWidget(&ircSettingsWidget_);
}

void SettingsDialog::show() {
    settingsDialog_.show();
}

void SettingsDialog::onProtocolSelected(const QString& text) {
    auto it = widgetMap_.find(text);
    if (it == widgetMap_.end()) return;
    settingsDialogUi_.protocolSettings->setCurrentWidget(*it);
}

void SettingsDialog::onIrcServerSelected(const QModelIndex& index) {
    auto* item = static_cast<TreeEntry*>(index.internalPointer());
    if (item->getTreeEntryType() == 's') {
        Server* server = static_cast<Server*>(item);
        ircSettingsUi_.hostsList->setModel(&server->getHostModel());
        ircSettingsUi_.nickList->setModel(&server->getNickModel());
    }
}

std::shared_ptr<Server> SettingsDialog::getSelectedServer() {
    auto index = ircSettingsUi_.serverList->selectionModel()->currentIndex();
    if (index.isValid()) {
        auto* item = static_cast<TreeEntry*>(index.internalPointer());
        if (item->getTreeEntryType() == 's')
            return std::static_pointer_cast<Server>(static_cast<Server*>(item)->shared_from_this());
    }
    return std::shared_ptr<Server>();
}

std::shared_ptr<Host> SettingsDialog::getSelectedHost() {
    auto index = ircSettingsUi_.hostsList->selectionModel()->currentIndex();
    if (index.isValid()) {
        auto* item = static_cast<TreeEntry*>(index.internalPointer());
        if (item->getTreeEntryType() == 'h')
            return std::static_pointer_cast<Host>(static_cast<Host*>(item)->shared_from_this());
    }
    return std::shared_ptr<Host>();
}

QString SettingsDialog::getSelectedNick() {
    auto index = ircSettingsUi_.serverList->selectionModel()->currentIndex();
    if (index.isValid())
        return *static_cast<QString*>(index.internalPointer());
    return "";
}
