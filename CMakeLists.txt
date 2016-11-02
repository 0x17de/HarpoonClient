cmake_minimum_required(VERSION 3.1.0)
project(HarpoonClient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets)
find_package(Qt5WebSockets)

set(SRC_CLIENT
    src/main.cpp
    src/ChatUi.cpp src/ChatUi.hpp
    src/TreeEntry.cpp src/TreeEntry.hpp
    src/Server.cpp src/Server.hpp
    src/Channel.cpp src/Channel.hpp
    src/BacklogView.cpp src/BacklogView.hpp
    src/UserGroup.cpp src/UserGroup.hpp
    src/User.cpp src/User.hpp
    src/ChatLine.cpp src/ChatLine.hpp
    src/HarpoonClient.cpp src/HarpoonClient.hpp
    src/channellist/ChannelTreeModel.cpp src/channellist/ChannelTreeModel.hpp
    src/userlist/UserTreeModel.cpp src/userlist/UserTreeModel.hpp
    )

qt5_add_resources(ICONS_SRC icons/icons.qrc)
qt5_wrap_cpp(CHANNELTREEMODEL_SRC src/channellist/ChannelTreeModel.hpp)
qt5_wrap_cpp(USERTREEMODEL_SRC src/userlist/UserTreeModel.hpp)
qt5_wrap_cpp(HARPOONCLIENT_SRC src/HarpoonClient.hpp)
qt5_wrap_cpp(SERVER_SRC src/Server.hpp)
qt5_wrap_cpp(BACKLOGVIEW_SRC src/BacklogView.hpp)
qt5_wrap_cpp(CHANNEL_SRC src/Channel.hpp)
qt5_wrap_cpp(CHATUI_SRC src/ChatUi.hpp)
qt5_wrap_ui(CHATUI_HEADERS ui_forms/client.ui)
qt5_wrap_ui(SERVERCONFIGUI_HEADERS ui_forms/serverConfigurationDialog.ui)
qt5_wrap_ui(SETTINGSUI_HEADERS ui_forms/settings.ui)
qt5_wrap_ui(IRCCONFIGUI_HEADERS ui_forms/ircSettings.ui)

add_executable(HarpoonClient
    ${SRC_CLIENT}
    ${CHATUI_HEADERS}
    ${CHATUI_SRC}
    ${SETTINGSUI_HEADERS}
    ${CHANNELTREEMODEL_SRC}
    ${USERTREEMODEL_SRC}
    ${HARPOONCLIENT_SRC}
    ${SERVER_SRC}
    ${BACKLOGVIEW_SRC}
    ${CHANNEL_SRC}
    ${ICONS_SRC}
    ${SERVERCONFIGUI_HEADERS}
    ${IRCCONFIGUI_HEADERS}
    )
target_include_directories(HarpoonClient PUBLIC src)
target_link_libraries(HarpoonClient Qt5::Widgets Qt5::WebSockets)
