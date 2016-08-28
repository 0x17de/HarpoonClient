#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"


ChatUi::ChatUi(Client& client)
{
    Ui::Client{}.setupUi(this);

    ChannelListWidget* channelWidget = findChild<ChannelListWidget*>("channels");

    auto* server1 = channelWidget->addServer("Test1");
    channelWidget->addChannel(server1, "#test");

    channelWidget->addServer("Test2");

    QTreeView* tree = findChild<QTreeView*>("users");

    show();
}
