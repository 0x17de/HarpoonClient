#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QObject>

#include "Server.hpp"
#include "HarpoonClient.hpp"
#include "ui_settings.h"
#include "ui_ircSettings.h"


class SettingsTypeModel;
class ServerTreeModel;
class Host; // will be added in server.hpp
class SettingsDialog : public QObject {
    Q_OBJECT

    ServerTreeModel& channelTreeModel_;
    SettingsTypeModel& settingsTypeModel_;
    Ui::SettingsDialog settingsDialogUi_;
    Ui::IrcSettings ircSettingsUi_;
    QDialog settingsDialog_;
    QWidget ircSettingsWidget_;

public:
    SettingsDialog(HarpoonClient& client,
                   ServerTreeModel& channelTreeModel,
                   SettingsTypeModel& settingsTypeModel);
    virtual ~SettingsDialog();
    void show();

public Q_SLOTS:
    void onIrcServerSelected(const QModelIndex& index);

signals:

};


#endif
