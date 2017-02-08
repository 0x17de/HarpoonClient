#include "IrcNickModel.hpp"
#include "moc_IrcNickModel.cpp"


IrcNickModel::IrcNickModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex IrcNickModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= nicks_.size())
            return QModelIndex();
        auto& entries = const_cast<std::list<QString>&>(nicks_);
        auto it = entries.begin();
        std::advance(it, row);
        return createIndex(row, column, &(*it));
    }
    return QModelIndex();
}

QModelIndex IrcNickModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int IrcNickModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return nicks_.size();

    return 0;
}

int IrcNickModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant IrcNickModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    QString* nick = static_cast<QString*>(index.internalPointer());

    if (role != Qt::DisplayRole)
        return QVariant();

    return *nick;
}

Qt::ItemFlags IrcNickModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IrcNickModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

void IrcNickModel::resetNicks(std::list<QString>& nicks) {
    beginResetModel();
    nicks_.clear();
    nicks_.insert(nicks_.begin(), nicks.begin(), nicks.end());
    endResetModel();
}

void IrcNickModel::newNick(const QString& nick) {
    int rowIndex = nicks_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    nicks_.push_back(nick);
    endInsertRows();
}

void IrcNickModel::deleteNick(const QString& nickName) {
    int rowIndex = 0;
    decltype(nicks_)::iterator it;
    for (it = nicks_.begin(); it != nicks_.end(); ++it, ++rowIndex) {
        if (*it == nickName)
            break;
    }
    if (it == nicks_.end()) return;
    beginRemoveRows(QModelIndex{}, rowIndex, rowIndex+1);
    nicks_.erase(it);
    endRemoveRows();
}

void IrcNickModel::modifyNick(const QString& oldNick, const QString& newNick) {
    if (newNick == "") {
        this->deleteNick(oldNick);
        return;
    } else if (oldNick == "") {
        this->newNick(newNick);
        return;
    }

    // modify existing entry
    int rowIndex = 0;
    decltype(nicks_)::iterator it;
    for (it = nicks_.begin(); it != nicks_.end(); ++it, ++rowIndex) {
        if (*it == oldNick)
            break;
    }
    if (it == nicks_.end()) return;

    *it = newNick;
    auto index = createIndex(rowIndex, 0, &(*it));
    emit dataChanged(index, index);
}
