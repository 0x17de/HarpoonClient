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
