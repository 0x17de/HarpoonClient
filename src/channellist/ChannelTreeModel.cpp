#include "ChannelTreeModel.hpp"
#include "../Server.hpp"
#include "../Channel.hpp"


ChannelTreeModel::ChannelTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex ChannelTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= servers_.size())
            return QModelIndex();
        auto it = servers_.begin();
        std::advance(it, row);
        return createIndex(row, column, (*it).get());
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            Server* server = static_cast<Server*>(parent.internalPointer());
            return createIndex(row, column, server->getChannel(row));
        }
    }
    return QModelIndex();
}

QModelIndex ChannelTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);
    if (item->getTreeEntryType() == 'c') {
        Channel* channel = static_cast<Channel*>(ptr);
        Server* server = channel->getServer();

        int rowIndex = server->getChannelIndex(channel);
        if (rowIndex >= 0)
            return createIndex(rowIndex, 0, server);
    }

    return QModelIndex();
}

int ChannelTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return servers_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            Server* server = static_cast<Server*>(parent.internalPointer());
            return server->getChannelCount();
        }
    }

    return 0;
}

int ChannelTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant ChannelTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);

    if (item->getTreeEntryType() == 's') {
        Server* server = static_cast<Server*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return server->getName();
    } else {
        Channel* channel = static_cast<Channel*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return channel->getName();
    }

    return QVariant();
}

Qt::ItemFlags ChannelTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant ChannelTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

void ChannelTreeModel::addServers(const std::list<std::shared_ptr<Server>>& newServers) {
    int start = servers_.size();
    int end = newServers.size();

    auto index = QModelIndex();

    beginInsertRows(index, start, start+end-1);

    servers_.insert(servers_.end(), newServers.begin(), newServers.end());

    endInsertRows();
}
