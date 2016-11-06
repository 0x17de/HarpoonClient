#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QObject>

#include "Server.hpp"
#include "ui_settings.h"
#include "ui_ircSettings.h"


class Host; // will be added in server.hpp
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
    void serverAdded(Server* server);
    void serverRemoved(Server* server);
    void serverReset(std::list<Server*> server);
    void hostAdded(Host* host);
    void hostRemoved(Host* host);
    void nickModified(Server* server, const std::string& oldNick, const std::string& newNick);

signals:

};


#endif
