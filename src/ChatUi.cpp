#include "ChatUi.hpp"
#include "ChatUi_Impl.hpp"

ChatUi::ChatUi()
    : impl{std::make_shared<ChatUi_Impl>()}
{
}
