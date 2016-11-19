#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QObject>

#include "Server.hpp"
#include "HarpoonClient.hpp"
#include "ui_settings.h"
#include "ui_ircSettings.h"


class ChannelTreeModel;
class Host; // will be added in server.hpp
class SettingsDialog : public QObject {
    Q_OBJECT

    ChannelTreeModel& channelTreeModel_;
    Ui::SettingsDialog settingsDialogUi_;
    Ui::IrcSettings ircSettingsUi_;
    QDialog settingsDialog_;
    QWidget ircSettingsWidget_;

public:
    SettingsDialog(HarpoonClient& client, ChannelTreeModel& channelTreeModel);
    virtual ~SettingsDialog();
    void show();

public Q_SLOTS:
signals:

};


#endif
