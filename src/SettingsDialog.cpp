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
    editServerEntryUi_.setupUi(&editServerEntryDialog_);
    editHostEntryUi_.setupUi(&editHostEntryDialog_);

    widgetMap_.insert("irc", &ircSettingsWidget_);
    settingsDialogUi_.protocolSelection->setModel(&settingsTypeModel);
    settingsDialogUi_.protocolSettings->addWidget(&ircSettingsWidget_);
    ircSettingsUi_.serverList->setModel(&channelTreeModel);

    connect(settingsDialogUi_.protocolSelection, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::activated), this, &SettingsDialog::onProtocolSelected);
    connect(ircSettingsUi_.serverList, &QListView::clicked, this, &SettingsDialog::onIrcServerSelected);

    // edit servers
    connect(ircSettingsUi_.btnNewServer, &QPushButton::clicked, [this]() {
            editServer_selectedServer.reset();;
            editServerEntryUi_.serverName->setText("");
            editServerEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnEditServer, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            if (!server) return;
            editServer_selectedServer = server;
            editServerEntryUi_.serverName->setText(server->getName());
            editServerEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnDeleteServer, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            client_.sendMessage(server.get(), nullptr, "/deleteserver");
        });
    connect(ircSettingsUi_.btnReconnect, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            client_.sendMessage(server.get(), nullptr, "/reconnect");
        });
    connect(editServerEntryUi_.buttonBox, &QDialogButtonBox::accepted, [this]() {
            QString serverName = editServerEntryUi_.serverName->text();
            auto server = editServer_selectedServer.lock();
            if (server) {
                client_.sendMessage(server.get(), nullptr, "/editserver "+serverName);
            } else {
                client_.sendMessage(nullptr, nullptr, "/addserver "+serverName);
            }
        });

    // edit hosts
    connect(ircSettingsUi_.btnNewHost, &QPushButton::clicked, [this]() {
            editHost_selectedHost.reset();;
            editHostEntryUi_.hostname->setText("");
            editHostEntryUi_.port->setText("");
            editHostEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnEditHost, &QPushButton::clicked, [this]() {
            auto host = getSelectedHost();
            if (!host) return;
            editHost_selectedHost = host;
            editHostEntryUi_.hostname->setText(host->getHost());
            editHostEntryUi_.port->setText(QString::number(host->getPort()));
            editHostEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnDeleteHost, &QPushButton::clicked, [this]() {
            auto host = getSelectedHost();
            auto server = host->getServer().lock();
            if (!server) return;
            client_.sendMessage(server.get(), nullptr, "/deletehost "+host->getHost()+":"+host->getPort());
        });
    connect(editHostEntryUi_.buttonBox, &QDialogButtonBox::accepted, [this]() {
            QString hostname = editHostEntryUi_.hostname->text();
            QString port = editHostEntryUi_.port->text();
            auto host = editHost_selectedHost.lock();
            if (!host) {
                client_.sendMessage(nullptr, nullptr, "/addhost "+hostname+":"+port);
                return;
            }
            auto server = host->getServer().lock();
            if (!server) return;
            client_.sendMessage(server.get(), nullptr, "/edithost "+host->getHost()+":"+host->getPort()+" "+hostname+":"+port);
        });

    // edit nicks
    connect(ircSettingsUi_.btnNewNick, &QPushButton::clicked, [this]() {
        });
    connect(ircSettingsUi_.btnEditNick, &QPushButton::clicked, [this]() {
        });
    connect(ircSettingsUi_.btnDeleteNick, &QPushButton::clicked, [this]() {
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
