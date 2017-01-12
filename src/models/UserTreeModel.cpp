#include "UserTreeModel.hpp"
#include "moc_UserTreeModel.cpp"
#include "../User.hpp"
#include "../UserGroup.hpp"


UserTreeModel::UserTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex UserTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= groups_.size())
            return QModelIndex();
        auto it = groups_.begin();
        std::advance(it, row);
        return createIndex(row, column, it->get());
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 'g') {
            UserGroup* userGroup = static_cast<UserGroup*>(parent.internalPointer());
            return createIndex(row, column, userGroup->getUser(row));
        }
    }
    return QModelIndex();
}

QModelIndex UserTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);
    if (item->getTreeEntryType() == 'u') {
        User* user = static_cast<User*>(ptr);
        UserGroup* userGroup = user->getUserGroup();

        int rowIndex = userGroup->getUserIndex(user);
        if (rowIndex >= 0)
            return createIndex(rowIndex, 0, userGroup);
    }

    return QModelIndex();
}

int UserTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return groups_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 'g') {
            UserGroup* userGroup = static_cast<UserGroup*>(parent.internalPointer());
            return userGroup->getUserCount();
        }
    }

    return 0;
}

int UserTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant UserTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);

    if (item->getTreeEntryType() == 'g') {
        UserGroup* userGroup = static_cast<UserGroup*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return userGroup->getName();
    } else {
        User* user = static_cast<User*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return user->getNick();
    }

    return QVariant();
}

Qt::ItemFlags UserTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant UserTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Users";

    return QVariant();
}

int UserTreeModel::getUserGroupIndex(UserGroup* userGroup) {
    int rowIndex = 0;
    for (auto& g : groups_) {
        if (g.get() == userGroup)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

User* UserTreeModel::getUser(QString nick) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<User>& user){
            return user->getNick() == nick;
        });
    return (it == users_.end() ? nullptr : (*it).get());
}
#include <QDebug>
void UserTreeModel::resetUsers(std::list<std::shared_ptr<User>>& users) {
    beginResetModel();
    groups_.clear();
    users_.swap(users);

    auto groupOwners = std::make_shared<UserGroup>("Owners");
    auto groupAdmins = std::make_shared<UserGroup>("Admins");
    auto groupOperators = std::make_shared<UserGroup>("Operators");
    auto groupHalfOperators = std::make_shared<UserGroup>("HalfOperators");
    auto groupVoiced = std::make_shared<UserGroup>("Voiced");
    groupUsers_ = std::make_shared<UserGroup>("Users");

    for (auto& u : users_) {
        char mode = u->getAccessMode();
        qDebug() << u->getNick() << " MODE " << QString::number(mode);
        UserGroup* group;

        switch(mode) {
        case 'q': group = groupOwners.get();        break;
        case 'a': group = groupAdmins.get();        break;
        case 'o': group = groupOperators.get();     break;
        case 'h': group = groupHalfOperators.get(); break;
        case 'v': group = groupVoiced.get();        break;
        default: group = groupUsers_.get();
        }

        group->addUser(u);
    }

    if (groupOwners->getUserCount() > 0)
        groups_.push_back(groupOwners);
    if (groupAdmins->getUserCount() > 0)
        groups_.push_back(groupAdmins);
    if (groupOperators->getUserCount() > 0)
        groups_.push_back(groupOperators);
    if (groupHalfOperators->getUserCount() > 0)
        groups_.push_back(groupHalfOperators);
    if (groupVoiced->getUserCount() > 0)
        groups_.push_back(groupVoiced);
    groups_.push_back(groupUsers_);

    endResetModel();

    // autoexpand groups
    int rowIndex = 0;
    for (auto& group : groups_) {
        emit expand(createIndex(rowIndex, 0, group.get()));
        rowIndex += 1;
    }
}

void UserTreeModel::addUser(std::shared_ptr<User> user) {
    UserGroup* userGroup = user->getUserGroup();
    if (userGroup == nullptr) {
        if (groups_.size() > 0) {
            userGroup = groups_.front().get();
        } else
            return;
    }

    auto idx = getUserGroupIndex(groupUsers_.get());
    auto rowIndex = userGroup->getUserCount();
    beginInsertRows(index(idx, 0), rowIndex, rowIndex);
    users_.push_back(user);
    groupUsers_->addUser(user);
    endInsertRows();
}

bool UserTreeModel::removeUser(const QString& nick) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<User>& user){
            return user->getNick() == nick;
        });
    if (it == users_.end()) return false;

    User* user = (*it).get();
    UserGroup* userGroup = user->getUserGroup();
    auto rowIndex = userGroup->getUserIndex(user);

    int idx = getUserGroupIndex(userGroup);
    if (idx == -1)
        return false;

    beginRemoveRows(index(idx, 0), rowIndex, rowIndex);
    users_.erase(it);
    userGroup->removeUser(user);
    endRemoveRows();

    return true;
}

bool UserTreeModel::renameUser(const QString& nick,
                               const QString& newNick) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<User>& user){
            return user->getNick() == nick;
        });
    if (it == users_.end()) return false;

    User* user = (*it).get();
    auto modelIndex = createIndex(user->getUserGroup()->getUserIndex(user), 0, user);
    user->rename(newNick);
    emit dataChanged(modelIndex, modelIndex);

    return true;
}
