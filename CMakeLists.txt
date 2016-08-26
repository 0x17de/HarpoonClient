cmake_minimum_required(VERSION 3.1.0)
project(HarpoonClient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets)

set(SRC_CLIENT
    src/main.cpp
    src/ChatUi.cpp src/ChatUi.hpp
    src/ChatUi_Impl.cpp src/ChatUi_Impl.hpp
    )

qt5_add_resources(ICONS_SRC icons/icons.qrc)
qt5_wrap_cpp(CHATUI_SRC src/ChatUi_Impl.hpp)
qt5_wrap_ui(CHATUI_HEADERS
    ui_forms/client.ui)

add_executable(HarpoonClient ${SRC_CLIENT} ${CHATUI_HEADERS} ${CHATUI_SRC} ${ICONS_SRC})
target_link_libraries(HarpoonClient Qt5::Widgets)
