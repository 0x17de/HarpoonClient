#include <QApplication>
#include "ChatUi.hpp"
#include "Client.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ChatUi ui;
    Client client;

    return app.exec();
}
