#include "ChatUi.hpp"

#include <QTreeWidget>
#include "ui_client.h"


ChatUi::ChatUi()
{
    Ui::Client{}.setupUi(this);

    QTreeView* tree = findChild<QTreeView*>("users");
    show();
}
