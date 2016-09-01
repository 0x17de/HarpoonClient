#include <QApplication>
#include "ChatUi.hpp"
#include "HarpoonClient.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    HarpoonClient client;
    ChatUi ui(client);
    client.run();

    return app.exec();
}
