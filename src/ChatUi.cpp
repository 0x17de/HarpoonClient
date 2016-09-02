#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "Channel.hpp"


ChatUi::ChatUi(HarpoonClient& client)
{
    Ui::Client{}.setupUi(this);
    connect(&client, &HarpoonClient::newServer, this, &ChatUi::newServer);
    connect(&client, &HarpoonClient::newChannel, this, &ChatUi::newChannel);

    findChild<QTreeView*>("channels")->setModel(&channelTreeModel);

    auto server1 = std::make_shared<Server>("TestServer1");
    auto channel1 = std::make_shared<Channel>(server1.get(), "#TestChannel");

    server1->addChannel(channel1);

    channelTreeModel.addServers({
        server1
    });

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}

void ChatUi::newServer(const QString& serverId, const QString& name) {

}

void ChatUi::newChannel(const QString& serverId, const QString& name) {

}
