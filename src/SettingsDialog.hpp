#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QObject>

#include "ui_settings.h"
#include "ui_ircSettings.h"


class SettingsDialog : public QObject {
    Q_OBJECT

    Ui::Settings settingsDialogUi_;
    Ui::IrcSettings ircSettingsUi_;
    QDialog settingsDialog_;
    QWidget ircSettingsWidget_;

public:
    SettingsDialog();
    virtual ~SettingsDialog();
    void show();

public Q_SLOTS:
signals:

};


#endif
