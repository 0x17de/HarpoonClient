#ifndef CHATUI_H
#define CHATUI_H

#include <QSettings>
#include <list>
#include <memory>
#include "SettingsDialog.hpp"
#include "ui_client.h"
#include "ui_serverConfigurationDialog.h"


class ServerTreeModel;
class SettingsTypeModel;
class Server;
class Channel;
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

    ServerTreeModel& serverTreeModel_;
    SettingsTypeModel& settingsTypeModel_;

    QLineEdit* topicView_;
    QTreeView* channelView_;
    QStackedWidget* userViews_;
    QStackedWidget* backlogViews_;
    QLineEdit* messageInputView_;
    Channel* activeChannel_;

    QDialog bouncerConfigurationDialog_;
    SettingsDialog settingsDialog_;

public:
    ChatUi(HarpoonClient& client,
           ServerTreeModel& serverTreeModel,
           SettingsTypeModel& settingsTypeModel);
    virtual ~ChatUi();

private:
    void activateChannel(Channel* channel);
    void showConfigureNetworksDialog();
    void showConfigureBouncerDialog();

signals:
    void sendMessage(Server* server, Channel* channel, const QString& message);

public Q_SLOTS:
    void onChannelViewSelection(const QModelIndex& index);
    void expandServer(const QModelIndex& index);
    void channelConnected(Channel* channel);
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void messageReturnPressed();
};


#endif
