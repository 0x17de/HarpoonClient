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
        return createIndex(row, 0, (*it).get());
    } else {
        Server* server = static_cast<Server*>(parent.internalPointer());
        return createIndex(row, 1, server->getChannel(row));
    }
}

QModelIndex ChannelTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    if (index.column() == 1) {
        Channel* channel = static_cast<Channel*>(index.internalPointer());

        int index = 0;
        for (auto s : servers_) {
            if (s.get() == channel->getServer())
                return createIndex(index, 0, s.get());
            ++index;
        }
    }
    return QModelIndex();
}

int ChannelTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return servers_.size();
    } else {
        auto* ptr = parent.internalPointer();
        if (ptr == nullptr)
            return servers_.size();
        if (parent.column() == 0) {
            Server* server = static_cast<Server*>(parent.internalPointer());;
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

    if (index.column() == 0) {
        Server* server = static_cast<Server*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return server->getName();
    } else if (index.column() == 1) {
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

    auto index = createIndex(0, 0, nullptr);

    beginInsertRows(index, start, start+end-1);

    servers_.insert(servers_.end(), newServers.begin(), newServers.end());

    endInsertRows();
}
