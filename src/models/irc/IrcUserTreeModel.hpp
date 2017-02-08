#ifndef USERTREEMODELIRC_H
#define USERTREEMODELIRC_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class IrcUser;
class IrcUserGroup;

class IrcUserTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit IrcUserTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    IrcUser* getUser(QString nick);
    int getUserGroupIndex(IrcUserGroup* userGroup);
    void reconnectEvents();
    void addUser(std::shared_ptr<IrcUser> user);
    bool removeUser(const QString& nick);
    bool renameUser(const QString& nick,
                    const QString& newNick);
    bool changeMode(const QString& nick,
                    char mode,
                    bool add);

private:
    IrcUserGroup* getGroup(const QString& name);
    QString modeName(char modeChar) const;

signals:
    void expand(const QModelIndex& index);

public Q_SLOTS:
    void resetUsers(std::list<std::shared_ptr<IrcUser>>& users);

private:
    std::shared_ptr<IrcUserGroup> groupUsers_;
    std::list<std::shared_ptr<IrcUserGroup>> groups_;
    std::list<std::shared_ptr<IrcUser>> users_;
};

#endif
