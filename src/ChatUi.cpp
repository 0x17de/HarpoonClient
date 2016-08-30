#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"
#include "channellist/ChannelTreeItem.hpp"


ChatUi::ChatUi(Client& client)
{
    Ui::Client{}.setupUi(this);
    connect(&client, &Client::newServer, this, &ChatUi::newServer);
    connect(&client, &Client::newChannel, this, &ChatUi::newChannel);

    channelWidget = findChild<ChannelListWidget*>("channels");

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}

void ChatUi::newServer(const QString& serverId, const QString& name) {
    auto* server1 = channelWidget->getRoot()->addServer(name);
}

void ChatUi::newChannel(const QString& serverId, const QString& name) {
    //auto* server1 = channelWidget->getRoot()->addServer(name);
}
