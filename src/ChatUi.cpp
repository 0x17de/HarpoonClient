#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "Channel.hpp"


ChatUi::ChatUi(HarpoonClient& client)
{
    Ui::Client{}.setupUi(this);
    connect(&client, &HarpoonClient::newServers, this, &ChatUi::newServers);
    connect(&client, &HarpoonClient::newChannel, this, &ChatUi::newChannel);

    findChild<QTreeView*>("channels")->setModel(&channelTreeModel);

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}

void ChatUi::newServers(const std::list<std::shared_ptr<Server>>& servers) {
    qDebug() << "SERVERS" << servers.size();
    channelTreeModel.addServers(servers);
}

void ChatUi::newChannel(const QString& serverId, const QString& name) {

}
