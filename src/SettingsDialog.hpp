#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <memory>

#include <QObject>
#include <QHash>

#include "irc/IrcServer.hpp"
#include "HarpoonClient.hpp"
#include "ui_settings.h"
#include "ui_ircSettings.h"
#include "ui_editServerEntry.h"
#include "ui_editHostEntry.h"
#include "ui_editNickEntry.h"


class SettingsTypeModel;
class IrcServerTreeModel;
class IrcHost; // will be added in server.hpp
class SettingsDialog : public QObject {
    Q_OBJECT

    HarpoonClient& client_;
    IrcServerTreeModel& channelTreeModel_;
    SettingsTypeModel& settingsTypeModel_;
    Ui::SettingsDialog settingsDialogUi_;
    Ui::IrcSettings ircSettingsUi_;
    QDialog settingsDialog_;
    QWidget ircSettingsWidget_;
    QHash<QString, QWidget*> widgetMap_;

    // server
    std::weak_ptr<IrcServer> editServer_selectedServer;
    Ui::EditServerEntry editServerEntryUi_;
    QDialog editServerEntryDialog_;
    // host
    std::weak_ptr<IrcServer> editHost_selectedServer;
    std::weak_ptr<IrcHost> editHost_selectedHost;
    Ui::EditHostEntry editHostEntryUi_;
    QDialog editHostEntryDialog_;
    // nick
    std::weak_ptr<IrcServer> editNick_selectedServer;
    QString editNick_selectedNick;
    Ui::EditNickEntry editNickEntryUi_;
    QDialog editNickEntryDialog_;


    std::shared_ptr<IrcServer> getSelectedServer();
    std::shared_ptr<IrcHost> getSelectedHost();
    QString getSelectedNick();

public:
    SettingsDialog(HarpoonClient& client,
                   IrcServerTreeModel& channelTreeModel,
                   SettingsTypeModel& settingsTypeModel);
    virtual ~SettingsDialog();
    void show();

public Q_SLOTS:
    void onProtocolSelected(const QString& text);
    void onIrcServerSelected(const QModelIndex& index);

signals:

};


#endif
