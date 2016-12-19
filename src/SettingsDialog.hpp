#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <memory>

#include <QObject>
#include <QHash>

#include "Server.hpp"
#include "HarpoonClient.hpp"
#include "ui_settings.h"
#include "ui_ircSettings.h"
#include "ui_editServerEntry.h"


class SettingsTypeModel;
class ServerTreeModel;
class Host; // will be added in server.hpp
class SettingsDialog : public QObject {
    Q_OBJECT

    HarpoonClient& client_;
    ServerTreeModel& channelTreeModel_;
    SettingsTypeModel& settingsTypeModel_;
    Ui::SettingsDialog settingsDialogUi_;
    Ui::IrcSettings ircSettingsUi_;
    QDialog settingsDialog_;
    QWidget ircSettingsWidget_;
    QHash<QString, QWidget*> widgetMap_;

    std::weak_ptr<Server> editServer_selectedServer;
    Ui::EditServerEntry editServerEntryUi_;
    QDialog editServerEntryDialog_;

    std::shared_ptr<Server> getSelectedServer();
    std::shared_ptr<Host> getSelectedHost();
    QString getSelectedNick();

public:
    SettingsDialog(HarpoonClient& client,
                   ServerTreeModel& channelTreeModel,
                   SettingsTypeModel& settingsTypeModel);
    virtual ~SettingsDialog();
    void show();

public Q_SLOTS:
    void onProtocolSelected(const QString& text);
    void onIrcServerSelected(const QModelIndex& index);

signals:

};


#endif
