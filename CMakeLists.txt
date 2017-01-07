cmake_minimum_required(VERSION 3.1.0)
project(HarpoonClient)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

file(STRINGS src/version.hpp VERSION)
string(REPLACE " " ";" VERSION "${VERSION}")
list(LENGTH VERSION VERSION_LEN)
list(GET VERSION 4 CPACK_PACKAGE_VERSION_MAJOR)
list(GET VERSION 10 CPACK_PACKAGE_VERSION_MINOR)
list(GET VERSION 16 CPACK_PACKAGE_VERSION_PATCH)
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
message("HarpoonClient Version ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

find_package(Qt5Widgets)
find_package(Qt5WebSockets)

set(SRC_CLIENT
    src/version.hpp
    src/main.cpp
    src/ChatUi.cpp src/ChatUi.hpp
    src/TreeEntry.cpp src/TreeEntry.hpp
    src/Server.cpp src/Server.hpp
    src/Host.cpp src/Host.hpp
    src/Channel.cpp src/Channel.hpp
    src/BacklogView.cpp src/BacklogView.hpp
    src/GraphicsHandle.cpp src/GraphicsHandle.hpp
    src/UserGroup.cpp src/UserGroup.hpp
    src/User.cpp src/User.hpp
    src/ChatLine.cpp src/ChatLine.hpp
    src/SettingsDialog.cpp src/SettingsDialog.hpp
    src/HarpoonClient.cpp src/HarpoonClient.hpp
    src/models/ServerTreeModel.cpp src/models/ServerTreeModel.hpp
    src/models/ChannelTreeModel.cpp src/models/ChannelTreeModel.hpp
    src/models/UserTreeModel.cpp src/models/UserTreeModel.hpp
    src/models/HostTreeModel.cpp src/models/HostTreeModel.hpp
    src/models/NickModel.cpp src/models/NickModel.hpp
    src/models/SettingsTypeModel.cpp src/models/SettingsTypeModel.hpp
    )

qt5_add_resources(ICONS_SRC icons/icons.qrc)

qt5_wrap_ui(ABOUT_HEADERS ui_forms/about.ui)
qt5_wrap_ui(CHATUI_HEADERS ui_forms/client.ui)
qt5_wrap_ui(EDITSERVERENTRY_HEADERS ui_forms/editServerEntry.ui)
qt5_wrap_ui(EDITHOSTENTRY_HEADERS ui_forms/editHostEntry.ui)
qt5_wrap_ui(EDITNICKENTRY_HEADERS ui_forms/editNickEntry.ui)
qt5_wrap_ui(SERVERCONFIGUI_HEADERS ui_forms/serverConfigurationDialog.ui)
qt5_wrap_ui(SETTINGSUI_HEADERS ui_forms/settings.ui)
qt5_wrap_ui(IRCCONFIGUI_HEADERS ui_forms/ircSettings.ui)

add_executable(HarpoonClient WIN32
    ${SRC_CLIENT}
    ${ABOUT_HEADERS}
    ${CHATUI_HEADERS}
    ${EDITSERVERENTRY_HEADERS}
    ${EDITHOSTENTRY_HEADERS}
    ${EDITNICKENTRY_HEADERS}
    ${SETTINGSUI_HEADERS}
    ${ICONS_SRC}
    ${SERVERCONFIGUI_HEADERS}
    ${IRCCONFIGUI_HEADERS}
    )
target_include_directories(HarpoonClient PUBLIC src)
target_link_libraries(HarpoonClient Qt5::Widgets Qt5::WebSockets)


# OS SPECIFIC INSTALL SETTINGS
if(WIN32)
  INSTALL(TARGETS HarpoonClient
          DESTINATION .)
  SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
  include(InstallRequiredSystemLibraries)

  # INCLUDE LIBRARIES IF PRESENT
  SET(CPACK_GENERATOR "NSIS")
  FILE(GLOB EXT_DLLS "ext/*.dll")
  if(EXT_DLLS)
    install(FILES ${EXT_DLLS}
            DESTINATION .)
  endif()
  SET(CPACK_NSIS_CONTACT "Manuel Herrmann <m@manuel-herrmann.de>")
  SET(CPACK_NSIS_URL_INFO_ABOUT "https://www.manuel-herrmann.de")

  SET(CPACK_NSIS_HELP_LINK "https://github.com/HarpoonOrg/HarpoonClient")
  SET(CPACK_NSIS_MENU_LINKS "HarpoonClient.exe" "HarpoonClient")
  #SET(CPACK_CREATE_DESKTOP_LINKS "HarpoonClient.exe")
elseif(UNIX)
  INSTALL(TARGETS HarpoonClient
          DESTINATION bin)
  SET(CPACK_GENERATOR "DEB;TGZ")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Manuel Herrmann")
  SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
  SET(CPACK_MONOLITHIC_INSTALL 1)
endif()
SET(CPACK_PACKAGE_VENDOR "Manuel Herrmann")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "HarpoonClient")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_EXECUTABLES "HarpoonClient" "HarpoonClient")

INCLUDE(CPack)
