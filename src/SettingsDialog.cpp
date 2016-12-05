#include "SettingsDialog.hpp"
#include "models/ServerTreeModel.hpp"
#include "models/SettingsTypeModel.hpp"
#include "HarpoonClient.hpp"


SettingsDialog::SettingsDialog(HarpoonClient& client,
                               ServerTreeModel& channelTreeModel,
                               SettingsTypeModel& settingsTypeModel)
    : channelTreeModel_{channelTreeModel}
    , settingsTypeModel_{settingsTypeModel}
{
    // settings dialog
    settingsDialogUi_.setupUi(&settingsDialog_);
    ircSettingsUi_.setupUi(&ircSettingsWidget_);

    settingsTypeModel.newType("irc", &ircSettingsWidget_);
    settingsDialogUi_.protocolSelection->setModel(&settingsTypeModel);
    settingsDialogUi_.protocolSettings->addWidget(&ircSettingsWidget_);
    ircSettingsUi_.serverList->setModel(&channelTreeModel);

    connect(ircSettingsUi_.serverList, &QListView::clicked, this, &SettingsDialog::onIrcServerSelected);
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

void SettingsDialog::onIrcServerSelected(const QModelIndex& index) {
    auto* item = static_cast<TreeEntry*>(index.internalPointer());
    if (item->getTreeEntryType() == 's') {
        Server* server = static_cast<Server*>(item);
        ircSettingsUi_.hostsList->setModel(&server->getHostModel());
    }
}
