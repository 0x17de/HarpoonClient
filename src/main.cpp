#include <QApplication>
#include "ChatUi.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ChatUi ui;

    return app.exec();
}
