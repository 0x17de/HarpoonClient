#ifndef USERTREEMODEL_H
#define USERTREEMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class User;
class UserGroup;

class UserTreeModel : public QAbstractItemModel {
    Q_OBJECT

 public:
    explicit UserTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    int getUserGroupIndex(UserGroup* userGroup);
    void reconnectEvents();

 public Q_SLOTS:
    void resetUsers(std::list<std::shared_ptr<User>>& users);
    void newUser(std::shared_ptr<User> user);

 private:
    std::list<UserGroup*> groups_;
    std::list<std::shared_ptr<User>> users_;
};

#endif
