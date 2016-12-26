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
    editNickEntryUi_.setupUi(&editNickEntryDialog_);

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
            auto server = getSelectedServer();
            if (!server) return;
            editHost_selectedServer = server;
            editHost_selectedHost.reset();
            editHostEntryUi_.hostname->setText("");
            editHostEntryUi_.port->setText("");
            editHostEntryUi_.cbSsl->setChecked(true);
            editHostEntryUi_.cbIpv6->setChecked(false);
            editHostEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnEditHost, &QPushButton::clicked, [this]() {
            auto host = getSelectedHost();
            if (!host) return;
            editHost_selectedHost = host;
            editHost_selectedServer = host->getServer();
            editHostEntryUi_.hostname->setText(host->getHost());
            editHostEntryUi_.port->setText(QString::number(host->getPort()));
            editHostEntryUi_.cbSsl->setChecked(host->getSsl());
            editHostEntryUi_.cbIpv6->setChecked(host->getIpv6());
            editHostEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnDeleteHost, &QPushButton::clicked, [this]() {
            auto host = getSelectedHost();
            auto server = host->getServer().lock();
            if (!server) return;
            client_.sendMessage(server.get(), nullptr, "/deletehost "+host->getHost()+" "+QString::number(host->getPort()));
        });
    connect(editHostEntryUi_.buttonBox, &QDialogButtonBox::accepted, [this]() {
            QString hostname = editHostEntryUi_.hostname->text();
            QString port = editHostEntryUi_.port->text();
            bool ssl = editHostEntryUi_.cbSsl->isChecked();
            bool ipv6 = editHostEntryUi_.cbIpv6->isChecked();
            auto server = editHost_selectedServer.lock();
            if (!server) return;
            auto host = editHost_selectedHost.lock();
            if (!host) {
                client_.sendMessage(server.get(), nullptr, "/addhost "+hostname+" "+port+" "+(ssl?"true":"false")+" "+(ipv6?"true":"false"));
            } else {
                auto server = host->getServer().lock();
                if (!server) return;
                client_.sendMessage(server.get(), nullptr, "/edithost "+host->getHost()+" "+host->getPort()+" "+hostname+" "+port+" "+(ssl?"true":"false")+" "+(ipv6?"true":"false"));
            }
        });

    // edit nicks
    connect(ircSettingsUi_.btnNewNick, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            if (!server) return;
            editNick_selectedServer = server;
            editNick_selectedNick = "";
            editNickEntryUi_.nick->setText("");
            editNickEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnEditNick, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            if (!server) return;
            editNick_selectedServer = server;
            QString nick = getSelectedNick();
            if (nick == "") return;
            editNick_selectedNick = nick;
            editNickEntryUi_.nick->setText(nick);
            editNickEntryDialog_.show();
        });
    connect(ircSettingsUi_.btnDeleteNick, &QPushButton::clicked, [this]() {
            auto server = getSelectedServer();
            auto nick = getSelectedNick();
            client_.sendMessage(server.get(), nullptr, "/deletenick "+nick);
        });
    connect(editNickEntryUi_.buttonBox, &QDialogButtonBox::accepted, [this]() {
            auto server = editNick_selectedServer.lock();
            if (!server) return;
            QString nick = editNickEntryUi_.nick->text();
            QString oldNick = editNick_selectedNick;
            if (oldNick != "") {
                client_.sendMessage(server.get(), nullptr, "/editnick "+oldNick+" "+nick);
            } else {
                client_.sendMessage(nullptr, nullptr, "/addnick "+nick);
            }
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
    auto index = ircSettingsUi_.nickList->selectionModel()->currentIndex();
    if (index.isValid())
        return *static_cast<QString*>(index.internalPointer());
    return "";
}
