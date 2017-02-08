#include "IrcUserTreeModel.hpp"
#include "moc_IrcUserTreeModel.cpp"
#include "irc/IrcUser.hpp"
#include "irc/IrcUserGroup.hpp"


IrcUserTreeModel::IrcUserTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex IrcUserTreeModel::index(int row, int column, const QModelIndex& parent) const {
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
            IrcUserGroup* userGroup = static_cast<IrcUserGroup*>(parent.internalPointer());
            return createIndex(row, column, userGroup->getUser(row));
        }
    }
    return QModelIndex();
}

QModelIndex IrcUserTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);
    if (item->getTreeEntryType() == 'u') {
        IrcUser* user = static_cast<IrcUser*>(ptr);
        IrcUserGroup* userGroup = user->getUserGroup();

        int rowIndex = userGroup->getUserIndex(user);
        if (rowIndex >= 0)
            return createIndex(rowIndex, 0, userGroup);
    }

    return QModelIndex();
}

int IrcUserTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return groups_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 'g') {
            IrcUserGroup* userGroup = static_cast<IrcUserGroup*>(parent.internalPointer());
            return userGroup->getUserCount();
        }
    }

    return 0;
}

int IrcUserTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant IrcUserTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);

    if (item->getTreeEntryType() == 'g') {
        IrcUserGroup* userGroup = static_cast<IrcUserGroup*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return userGroup->getName();
    } else {
        IrcUser* user = static_cast<IrcUser*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return user->getNick();
    }

    return QVariant();
}

Qt::ItemFlags IrcUserTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IrcUserTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Users";

    return QVariant();
}

int IrcUserTreeModel::getUserGroupIndex(IrcUserGroup* userGroup) {
    int rowIndex = 0;
    for (auto& g : groups_) {
        if (g.get() == userGroup)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

IrcUser* IrcUserTreeModel::getUser(QString nick) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<IrcUser>& user){
            return user->getNick() == nick;
        });
    return (it == users_.end() ? nullptr : (*it).get());
}

IrcUserGroup* IrcUserTreeModel::getGroup(const QString& name) {
    auto it = find_if(groups_.begin(), groups_.end(), [&name](const std::shared_ptr<IrcUserGroup>& group){
            return group->getName() == name;
        });
    if (it == groups_.end()) {
        auto rowIndex = groups_.size();
        beginInsertRows(QModelIndex(), rowIndex, rowIndex);
        auto group = std::make_shared<IrcUserGroup>(name);
        groups_.push_back(group);
        endInsertRows();
        return group.get();
    }
    return (*it).get();
}

QString IrcUserTreeModel::modeName(char modeChar) const {
    QString modeName;
    switch(modeChar) {
    case 'q': modeName = "Owners"; break;
    case 'a': modeName = "Admins"; break;
    case 'o': modeName = "Operators"; break;
    case 'h': modeName = "HalfOperators"; break;
    case 'v': modeName = "Voiced"; break;
    default: modeName = "Users";
    }
    return modeName;
}

void IrcUserTreeModel::resetUsers(std::list<std::shared_ptr<IrcUser>>& users) {
    beginResetModel();
    groups_.clear();
    users_.swap(users);

    auto groupOwners = std::make_shared<IrcUserGroup>("Owners");
    auto groupAdmins = std::make_shared<IrcUserGroup>("Admins");
    auto groupOperators = std::make_shared<IrcUserGroup>("Operators");
    auto groupHalfOperators = std::make_shared<IrcUserGroup>("HalfOperators");
    auto groupVoiced = std::make_shared<IrcUserGroup>("Voiced");
    groupUsers_ = std::make_shared<IrcUserGroup>("Users");

    for (auto& u : users_) {
        char mode = u->getAccessMode();
        IrcUserGroup* group;

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

void IrcUserTreeModel::addUser(std::shared_ptr<IrcUser> user) {
    IrcUserGroup* userGroup = user->getUserGroup();
    if (userGroup == nullptr) {
        if (groups_.size() > 0) {
            userGroup = groupUsers_.get();
        } else
            return;
    }

    auto idx = getUserGroupIndex(groupUsers_.get());
    auto rowIndex = userGroup->getUserCount();
    beginInsertRows(index(idx, 0), rowIndex, rowIndex);
    users_.push_back(user);
    userGroup->addUser(user);
    endInsertRows();
}

bool IrcUserTreeModel::removeUser(const QString& nick) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<IrcUser>& user){
            return user->getNick() == nick;
        });
    if (it == users_.end()) return false;

    IrcUser* user = (*it).get();
    IrcUserGroup* userGroup = user->getUserGroup();

    int idx = getUserGroupIndex(userGroup);
    if (idx == -1)
        return false;

    if (userGroup == groupUsers_.get() || userGroup->getUserCount() > 1) {
        auto rowIndex = userGroup->getUserIndex(user);
        beginRemoveRows(index(idx, 0), rowIndex, rowIndex);
        users_.erase(it);
        userGroup->removeUser(user);
        endRemoveRows();
    } else {
        auto rowIndex = getUserGroupIndex(userGroup);
        beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
        users_.erase(it);
        userGroup->removeUser(user);
        auto it = groups_.begin();
        advance(it, rowIndex);
        groups_.erase(it);
        endRemoveRows();
    }

    return true;
}

bool IrcUserTreeModel::changeMode(const QString& nick,
                               char mode,
                               bool add) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<IrcUser>& user){
            return user->getNick() == nick;
        });
    if (it == users_.end()) return false;

    std::shared_ptr<IrcUser> user = *it;
    user->changeMode(mode, add);
    removeUser(nick);
    user->setUserGroup(getGroup(modeName(user->getAccessMode())));
    addUser(user);
}

bool IrcUserTreeModel::renameUser(const QString& nick,
                               const QString& newNick) {
    auto it = find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<IrcUser>& user){
            return user->getNick() == nick;
        });
    if (it == users_.end()) return false;

    IrcUser* user = (*it).get();
    auto modelIndex = createIndex(user->getUserGroup()->getUserIndex(user), 0, user);
    user->rename(newNick);
    emit dataChanged(modelIndex, modelIndex);

    return true;
}
