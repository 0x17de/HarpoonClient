cmake_minimum_required(VERSION 3.1.0)
project(HarpoonClient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets)
find_package(Qt5WebSockets)

set(SRC_CLIENT
    src/main.cpp
    src/ChatUi.cpp src/ChatUi.hpp
    src/Client.cpp src/Client.hpp
    src/channellist/ChannelListWidget.cpp src/channellist/ChannelListWidget.hpp
    src/channellist/ChannelTreeItem.cpp src/channellist/ChannelTreeItem.hpp
    src/channellist/ChannelTreeModel.cpp src/channellist/ChannelTreeModel.hpp
    )

qt5_add_resources(ICONS_SRC icons/icons.qrc)
qt5_wrap_cpp(CHATUI_SRC
    src/ChatUi.hpp)
qt5_wrap_cpp(CLIENT_SRC
    src/Client.hpp)
qt5_wrap_ui(CHATUI_HEADERS
    ui_forms/client.ui)

add_executable(HarpoonClient
    ${SRC_CLIENT}
    ${CHATUI_HEADERS}
    ${CHATUI_SRC}
    ${CLIENT_SRC}
    ${ICONS_SRC}
    )
target_include_directories(HarpoonClient PUBLIC src)
target_link_libraries(HarpoonClient Qt5::Widgets Qt5::WebSockets)
