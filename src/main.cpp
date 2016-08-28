#include <QApplication>
#include "ChatUi.hpp"
#include "Client.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Client client;
    ChatUi ui(client);
    client.run();

    return app.exec();
}
