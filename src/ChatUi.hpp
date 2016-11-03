#ifndef CHATUI_H
#define CHATUI_H

#include <QSettings>
#include <list>
#include <memory>
#include "channellist/ChannelTreeModel.hpp"
#include "ui_client.h"
#include "ui_serverConfigurationDialog.h"
#include "ui_settings.h"
#include "ui_ircSettings.h"


class Server;
class Channel;
class HarpoonClient;
class QTreeView;
class QTableView;
class QLineEdit;
class QStackedWidget;

class ChatUi : public QMainWindow {
    Q_OBJECT

    Ui::Client clientUi;
    Ui::ServerConfigurationDialog serverConfigurationDialogUi;
    Ui::Settings settingsDialogUi;
    Ui::IrcSettings ircSettingsUi;

    QSettings& settings;
    HarpoonClient& client;

    ChannelTreeModel channelTreeModel;
    QLineEdit* topicView;
    QTreeView* channelView;
    QStackedWidget* userViews;
    QStackedWidget* backlogViews;
    QLineEdit* messageInputView;
    Channel* activeChannel;

    QDialog serverConfigurationDialog;
    QDialog settingsDialog;
    QWidget ircSettingsWidget;

public:
    ChatUi(HarpoonClient& client);
    ~ChatUi();

private:
    void activateChannel(Channel* channel);
    void showConfigureServerDialog();

signals:
    void sendMessage(Channel* channel, const QString& message);

public Q_SLOTS:
    void onChannelViewSelection(const QModelIndex& index);
    void expandServer(const QModelIndex& index);
    void channelConnected(Channel* channel);
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void messageReturnPressed();
};


#endif
