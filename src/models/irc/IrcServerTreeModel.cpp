#include "IrcServerTreeModel.hpp"
#include "moc_IrcServerTreeModel.cpp"
#include "irc/IrcServer.hpp"
#include "irc/IrcChannel.hpp"

#include <QIcon>


IrcServerTreeModel::IrcServerTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex IrcServerTreeModel::index(int row, int column, const QModelIndex& parent) const {
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
            IrcServer* server = static_cast<IrcServer*>(parent.internalPointer());
            return createIndex(row, column, server->getChannelModel().getChannel(row));
        }
    }
    return QModelIndex();
}

QModelIndex IrcServerTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<TreeEntry*>(ptr);
    if (item->getTreeEntryType() == 'c') {
        IrcChannel* channel = static_cast<IrcChannel*>(ptr);
        std::shared_ptr<IrcServer> server = channel->getServer().lock();
        if (!server) return QModelIndex();

        int rowIndex = server->getChannelModel().getChannelIndex(channel);
        if (rowIndex >= 0)
            return createIndex(rowIndex, 0, server.get());
    }

    return QModelIndex();
}

int IrcServerTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return servers_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            IrcServer* server = static_cast<IrcServer*>(parent.internalPointer());
            return server->getChannelModel().rowCount();
        }
    }

    return 0;
}

int IrcServerTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant IrcServerTreeModel::data(const QModelIndex& index, int role) const {
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
        IrcChannel* channel = static_cast<IrcChannel*>(index.internalPointer());

        if (role == Qt::DecorationRole)
            return QIcon(channel->getDisabled() ? ":icons/channelDisabled.png" : ":icons/channel.png");

        if (role != Qt::DisplayRole)
            return QVariant();

        return channel->getName();
    }

    return QVariant();
}

Qt::ItemFlags IrcServerTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IrcServerTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

std::list<std::shared_ptr<IrcServer>>& IrcServerTreeModel::getServers() {
    return servers_;
}

std::shared_ptr<IrcServer> IrcServerTreeModel::getServer(const QString& serverId) {
    auto it = find_if(servers_.begin(), servers_.end(), [&serverId](std::shared_ptr<IrcServer> server){
            return server->getId() == serverId;
        });
    if (it == servers_.end()) return nullptr;
    return *it;
}

int IrcServerTreeModel::getServerIndex(IrcServer* server) {
    int rowIndex = 0;
    for (auto s : servers_) {
        if (s.get() == server)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

void IrcServerTreeModel::connectServer(IrcServer* server) {
    IrcChannelTreeModel& channelTreeModel = server->getChannelModel();
    connect(&channelTreeModel, &IrcChannelTreeModel::beginInsertChannel, [this](std::shared_ptr<IrcServer> server, int where) {
            beginInsertRows(index(getServerIndex(server.get()), 0), where, where);
        });
    connect(&channelTreeModel, &IrcChannelTreeModel::newChannel, [this](std::shared_ptr<IrcChannel> channel) {
            emit newChannel(channel);
        });
    connect(&channelTreeModel, &IrcChannelTreeModel::endInsertChannel, [this]() {
            endInsertRows();
        });
    connect(&channelTreeModel, &IrcChannelTreeModel::beginRemoveChannel, [this](std::shared_ptr<IrcServer> server, int where) {
            beginRemoveRows(index(getServerIndex(server.get()), 0), where, where);
        });
    connect(&channelTreeModel, &IrcChannelTreeModel::endRemoveChannel, [this]() {
            endRemoveRows();
        });
    connect(&channelTreeModel, static_cast<void (IrcChannelTreeModel::*)(std::shared_ptr<IrcServer>, int)>(&IrcChannelTreeModel::channelDataChanged), [this](std::shared_ptr<IrcServer> server, int where) {
            auto modelIndex = index(where, 0, index(getServerIndex(server.get()), 0));
            emit dataChanged(modelIndex, modelIndex);
        });
    for (auto& channel : server->getChannelModel().getChannels()) {
        emit newChannel(channel);
    }
}

void IrcServerTreeModel::resetServers(std::list<std::shared_ptr<IrcServer>>& servers) {
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

void IrcServerTreeModel::newServer(std::shared_ptr<IrcServer> server) {
    int rowIndex = servers_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);

    connectServer(server.get());

    servers_.push_back(server);
    endInsertRows();
}

void IrcServerTreeModel::deleteServer(const QString& serverId) {
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
