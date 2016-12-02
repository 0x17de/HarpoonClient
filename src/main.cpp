#include <QApplication>
#include "ChatUi.hpp"
#include "HarpoonClient.hpp"
#include "models/ServerTreeModel.hpp"
#include "models/SettingsTypeModel.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ServerTreeModel serverTreeModel;
    SettingsTypeModel settingsTypeModel;

    HarpoonClient client(serverTreeModel, settingsTypeModel);
    ChatUi ui(client, serverTreeModel, settingsTypeModel);
    client.run();

    return app.exec();
}
