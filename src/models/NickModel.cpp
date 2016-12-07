#include "NickModel.hpp"


NickTreeModel::NickTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex NickTreeModel::index(int row, int column, const QModelIndex& parent) const {
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

QModelIndex NickTreeModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int NickTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return nicks_.size();

    return 0;
}

int NickTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant NickTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    QString* nick = static_cast<QString*>(index.internalPointer());

    if (role != Qt::DisplayRole)
        return QVariant();

    return *nick;
}

Qt::ItemFlags NickTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant NickTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

void NickTreeModel::resetNicks(std::list<QString>& nicks) {
    beginResetModel();
    nicks_.clear();
    nicks_.insert(nicks_.begin(), nicks.begin(), nicks.end());
    endResetModel();
}

void NickTreeModel::newNick(const QString& nick) {
    int rowIndex = nicks_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    nicks_.push_back(nick);
    endInsertRows();
}

void NickTreeModel::deleteNick(const QString& nickName, int port) {
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
