#include <QApplication>
#include "ChatUi.hpp"
#include "HarpoonClient.hpp"
#include "models/ServerTreeModel.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ServerTreeModel serverTreeModel;
    HarpoonClient client(serverTreeModel);
    ChatUi ui(client, serverTreeModel);
    client.run();

    return app.exec();
}
