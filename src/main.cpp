#include <QApplication>
#include "ChatUi.hpp"
#include "HarpoonClient.hpp"
#include "models/irc/IrcServerTreeModel.hpp"
#include "models/SettingsTypeModel.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    IrcServerTreeModel ircServerTreeModel;
    SettingsTypeModel settingsTypeModel;

    HarpoonClient client(ircServerTreeModel, settingsTypeModel);
    ChatUi ui(client, ircServerTreeModel, settingsTypeModel);
    client.run();

    return app.exec();
}
