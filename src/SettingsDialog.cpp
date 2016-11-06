#include "SettingsDialog.hpp"


SettingsDialog::SettingsDialog() {
    // settings dialog
    settingsDialogUi_.setupUi(&settingsDialog_);
    ircSettingsUi_.setupUi(&ircSettingsWidget_);

    settingsDialogUi_.protocolSettings->addWidget(&ircSettingsWidget_);
}

SettingsDialog::~SettingsDialog() {
}

void SettingsDialog::show() {
    settingsDialog_.show();
}

void SettingsDialog::serverAdded(Server* server) {
    // TODO: handle signal
}
void SettingsDialog::serverRemoved(Server* server) {
    // TODO: handle signal
}
void SettingsDialog::serverReset(std::list<Server*> server) {
    // TODO: handle signal
}
void SettingsDialog::hostAdded(Host* host) {
    // TODO: handle signal
}
void SettingsDialog::hostRemoved(Host* host) {
    // TODO: handle signal
}
void SettingsDialog::nickModified(Server* server, const std::string& oldNick, const std::string& newNick) {
    // TODO: handle signal
}

