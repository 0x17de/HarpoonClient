#include <QApplication>
#include "ChatUi_Impl.hpp"
#include "ui_client.h"

ChatUi_Impl::ChatUi_Impl()
{
    Ui::Client uiClient;
    uiClient.setupUi(this);
    show();
}
