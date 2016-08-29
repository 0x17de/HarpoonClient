#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"
#include "channellist/ChannelTreeItem.hpp"


ChatUi::ChatUi(Client& client)
{
    Ui::Client{}.setupUi(this);

    ChannelListWidget* channelWidget = findChild<ChannelListWidget*>("channels");

    auto* server1 = channelWidget->getRoot()->addServer("Test1");
    server1->addChannel("#test");
    server1->addChannel("someuser", true);
    channelWidget->getRoot()->addServer("Test2");

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}
