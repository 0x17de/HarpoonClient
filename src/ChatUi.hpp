#ifndef CHATUI_H
#define CHATUI_H

#include <QMainWindow>
#include "Client.hpp"


class ChatUi : public QMainWindow {
public:
    ChatUi(Client& client);
};


#endif
