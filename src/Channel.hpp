#ifndef CHANNEL_H
#define CHANNEL_H


#include <QString>
#include <QTableView>
#include <QTreeView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFont>
#include <list>
#include <memory>

#include "ChatLine.hpp"
#include "TreeEntry.hpp"
#include "userlist/UserTreeModel.hpp"


enum class MessageColor {
    Default,
    Notice
};

class Server;
class Channel : public TreeEntry {
    Q_OBJECT

    Server* server_;
    QString name_;
    QString topic_;
    UserTreeModel userTreeModel_;
    bool disabled_;
    QTreeView userTreeView_;
    QGraphicsScene backlogScene_; // TODO: create own class + chat line class
    QGraphicsView backlogCanvas_;

    std::array<qreal, 3> splitting_;
    std::list<ChatLine> chatLines_;

    void resizeLines();

public:
    Channel(Server* server,
            const QString& name,
            bool disabled);

    Server* getServer() const;
    QString getName() const;
    void setTopic(const QString& topic);
    QString getTopic() const;
    bool getDisabled() const;
    void setDisabled(bool disabled);
    void addUser(std::shared_ptr<User> user);
    void addMessage(const QString& time,
                    const QString& nick,
                    const QString& message,
                    const MessageColor color = MessageColor::Default);
    void resetUsers(std::list<std::shared_ptr<User>>& users);
    User* getUser(QString nick);
    QGraphicsView* getBacklogView();
    QTreeView* getUserTreeView();
    UserTreeModel* getUserTreeModel();

public Q_SLOTS:
    void expandUserGroup(const QModelIndex& index);

signals:
    void channelDataChanged(Channel* channel);
    void beginAddUser(User* user);
    void endAddUser();
};


#endif
