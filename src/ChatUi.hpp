#ifndef CHATUI_H
#define CHATUI_H

#include <memory>


class ChatUi_Impl;
class ChatUi {
    std::shared_ptr<ChatUi_Impl> impl;
public:
    ChatUi();
};

#endif
