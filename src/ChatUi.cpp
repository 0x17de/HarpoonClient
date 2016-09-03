#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"
#include "HarpoonClient.hpp"

#include "Server.hpp"
#include "Channel.hpp"


ChatUi::ChatUi(HarpoonClient& client)
    : channelTreeModel(client.getServerListReference())
{
    Ui::Client{}.setupUi(this);
    connect(&client, &HarpoonClient::resetServers, &channelTreeModel, &ChannelTreeModel::resetServers);
    //connect(&client, &HarpoonClient::newChannel, this, &ChatUi::newChannel);

    channelView = findChild<QTreeView*>("channels");
    channelView->setModel(&channelTreeModel);

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}
