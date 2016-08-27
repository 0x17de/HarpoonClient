#ifndef CLIENT_H
#define CLIENT_H

#include <QWebSocket>
#include <QString>


class Client : public QObject {
    Q_OBJECT

public:
    Client();

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onMessage(QString message);

private:
    QWebSocket ws_;
};

#endif
