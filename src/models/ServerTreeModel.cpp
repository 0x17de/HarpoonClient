#include "ServerTreeModel.hpp"
#include "../Server.hpp"
#include "../Channel.hpp"

#include <QIcon>


ServerTreeModel::ServerTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex ServerTreeModel::index(int row, int column, const QModelIndex& parent) const {
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
            return createIndex(row, column, server->getChannelModel().getChannel(row));
        }
    }
    return QModelIndex();
}

QModelIndex ServerTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);
    if (item->getTreeEntryType() == 'c') {
        Channel* channel = static_cast<Channel*>(ptr);
        Server* server = channel->getServer();

        int rowIndex = server->getChannelModel().getChannelIndex(channel);
        if (rowIndex >= 0)
            return createIndex(rowIndex, 0, server);
    }

    return QModelIndex();
}

int ServerTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return servers_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            Server* server = static_cast<Server*>(parent.internalPointer());
            return server->getChannelModel().rowCount();
        }
    }

    return 0;
}

int ServerTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant ServerTreeModel::data(const QModelIndex& index, int role) const {
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
            return QIcon(channel->getDisabled() ? ":icons/channelDisabled.png" : ":icons/channel.png");

        if (role != Qt::DisplayRole)
            return QVariant();

        return channel->getName();
    }

    return QVariant();
}

Qt::ItemFlags ServerTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant ServerTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

std::list<std::shared_ptr<Server>> ServerTreeModel::getServers() {
    return servers_;
}

Server* ServerTreeModel::getServer(const QString& serverId) {
    auto it = find_if(servers_.begin(), servers_.end(), [&serverId](std::shared_ptr<Server> server){
            return server->getId() == serverId;
        });
    if (it == servers_.end()) return nullptr;
    return it->get();
}

int ServerTreeModel::getServerIndex(Server* server) {
    int rowIndex = 0;
    for (auto s : servers_) {
        if (s.get() == server)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

void ServerTreeModel::connectServer(Server* server) {
    ChannelTreeModel& channelTreeModel = server->getChannelModel();
    connect(&channelTreeModel, &ChannelTreeModel::beginInsertChannel, [this](Server* server, int where) {
            beginInsertRows(index(getServerIndex(server), 0), where, where);
        });
    connect(&channelTreeModel, &ChannelTreeModel::endInsertChannel, [this]() {
            endInsertRows();
        });
    connect(&channelTreeModel, &ChannelTreeModel::beginRemoveChannel, [this](Server* server, int where) {
            beginRemoveRows(index(getServerIndex(server), 0), where, where);
        });
    connect(&channelTreeModel, &ChannelTreeModel::endRemoveChannel, [this]() {
            endRemoveRows();
        });
}

void ServerTreeModel::resetServers(std::list<std::shared_ptr<Server>>& servers) {
    beginResetModel();
    servers_.clear();
    servers_.insert(servers_.begin(), servers.begin(), servers.end());
    for(auto s : servers_)
        connectServer(s.get());
    endResetModel();

    // autoexpand servers
    int rowIndex = 0;
    for (auto& server : servers_) {
        emit expand(createIndex(rowIndex, 0, server.get()));
        rowIndex += 1;
    }
}

void ServerTreeModel::newServer(std::shared_ptr<Server> server) {
    int rowIndex = servers_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);

    connectServer(server.get());

    servers_.push_back(server);
    endInsertRows();
}

void ServerTreeModel::deleteServer(const QString& serverId) {
    int rowIndex = 0;
    decltype(servers_)::iterator it;
    for (it = servers_.begin(); it != servers_.end(); ++it, ++rowIndex) {
        if ((*it)->getId() == serverId)
            break;
    }
    if (it == servers_.end()) return;
    beginRemoveRows(QModelIndex{}, rowIndex, rowIndex+1);
    servers_.erase(it);
    endRemoveRows();
}
