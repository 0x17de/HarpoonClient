#ifndef CHATUI_H
#define CHATUI_H

#include <QSettings>
#include <list>
#include <memory>
#include "SettingsDialog.hpp"
#include "ui_client.h"
#include "ui_about.h"
#include "ui_serverConfigurationDialog.h"


class IrcServerTreeModel;
class SettingsTypeModel;
class IrcServer;
class IrcChannel;
class HarpoonClient;
class QTreeView;
class QTableView;
class QLineEdit;
class QStackedWidget;


class ChatUi : public QMainWindow {
    Q_OBJECT

    Ui::Client clientUi_;
    Ui::ServerConfigurationDialog bouncerConfigurationDialogUi_;

    QSettings& settings_;
    HarpoonClient& client_;

    IrcServerTreeModel& serverTreeModel_;
    SettingsTypeModel& settingsTypeModel_;

    QLineEdit* topicView_;
    QTreeView* channelView_;
    QStackedWidget* userViews_;
    QStackedWidget* backlogViews_;
    QLineEdit* messageInputView_;
    IrcChannel* activeChannel_;

    QDialog bouncerConfigurationDialog_;
    SettingsDialog settingsDialog_;

    Ui::AboutDialog aboutDialogUi_;
    QDialog aboutDialog_;

public:
    ChatUi(HarpoonClient& client,
           IrcServerTreeModel& serverTreeModel,
           SettingsTypeModel& settingsTypeModel);
    virtual ~ChatUi();

private:
    void activateChannel(IrcChannel* channel);
    void showConfigureNetworksDialog();
    void showConfigureBouncerDialog();

signals:
    void sendMessage(IrcServer* server, IrcChannel* channel, const QString& message);

public Q_SLOTS:
    void showChannelContextMenu(const QPoint&);
    void onChannelViewSelection(const QModelIndex& index);
    void expandServer(const QModelIndex& index);
    void resetServers(std::list<std::shared_ptr<IrcServer>>& servers);
    void messageReturnPressed();
};


#endif
