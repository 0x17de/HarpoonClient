#include "IrcHostTreeModel.hpp"
#include "moc_IrcHostTreeModel.cpp"
#include "irc/IrcServer.hpp"
#include "irc/IrcHost.hpp"

#include <QIcon>


IrcHostTreeModel::IrcHostTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex IrcHostTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= hosts_.size())
            return QModelIndex();
        auto it = hosts_.begin();
        std::advance(it, row);
        return createIndex(row, column, (*it).get());
    }
    return QModelIndex();
}

QModelIndex IrcHostTreeModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int IrcHostTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return hosts_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            IrcServer* server = static_cast<IrcServer*>(parent.internalPointer());
            return server->getHostModel().rowCount();
        }
    }

    return 0;
}

int IrcHostTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant IrcHostTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);

    if (item->getTreeEntryType() == 's') {
        IrcServer* server = static_cast<IrcServer*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return server->getName();
    } else {
        IrcHost* host = static_cast<IrcHost*>(index.internalPointer());

        if (role != Qt::DisplayRole)
            return QVariant();

        return host->getHost() + ":" + QString::number(host->getPort());
    }

    return QVariant();
}

Qt::ItemFlags IrcHostTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IrcHostTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

void IrcHostTreeModel::resetHosts(std::list<std::shared_ptr<IrcHost>>& hosts) {
    beginResetModel();
    hosts_.clear();
    hosts_.insert(hosts_.begin(), hosts.begin(), hosts.end());
    endResetModel();
}

void IrcHostTreeModel::newHost(std::shared_ptr<IrcHost> host) {
    int rowIndex = hosts_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    hosts_.push_back(host);
    endInsertRows();
}

void IrcHostTreeModel::deleteHost(const QString& hostName, int port) {
    int rowIndex = 0;
    decltype(hosts_)::iterator it;
    for (it = hosts_.begin(); it != hosts_.end(); ++it, ++rowIndex) {
        if ((*it)->getHost() == hostName && (*it)->getPort() == port)
            break;
    }
    if (it == hosts_.end()) return;
    beginRemoveRows(QModelIndex{}, rowIndex, rowIndex+1);
    hosts_.erase(it);
    endRemoveRows();
}
