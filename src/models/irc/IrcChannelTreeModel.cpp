#include "IrcChannelTreeModel.hpp"
#include "moc_IrcChannelTreeModel.cpp"
#include "irc/IrcServer.hpp"
#include "irc/IrcChannel.hpp"

#include <QIcon>


IrcChannelTreeModel::IrcChannelTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex IrcChannelTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= channels_.size())
            return QModelIndex();
        auto it = channels_.begin();
        std::advance(it, row);
        return createIndex(row, column, (*it).get());
    }
    return QModelIndex();
}

QModelIndex IrcChannelTreeModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int IrcChannelTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return channels_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            IrcServer* server = static_cast<IrcServer*>(parent.internalPointer());
            return server->getChannelModel().rowCount();
        }
    }

    return 0;
}

int IrcChannelTreeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant IrcChannelTreeModel::data(const QModelIndex& index, int role) const {
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

Qt::ItemFlags IrcChannelTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IrcChannelTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Chats";

    return QVariant();
}

std::list<std::shared_ptr<IrcChannel>> IrcChannelTreeModel::getChannels() {
    return channels_;
}

IrcChannel* IrcChannelTreeModel::getChannel(const QString& channelName) {
    auto it = find_if(channels_.begin(), channels_.end(), [&channelName](std::shared_ptr<IrcChannel> channel) {
            return channel->getName() == channelName;
        });
    if (it == channels_.end()) return nullptr;
    return it->get();
}

IrcChannel* IrcChannelTreeModel::getChannel(int row) {
    return static_cast<IrcChannel*>(index(row, 0).internalPointer());
}

int IrcChannelTreeModel::getChannelIndex(IrcChannel* channel) {
    int rowIndex = 0;
    for (auto s : channels_) {
        if (s.get() == channel)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

int IrcChannelTreeModel::getChannelIndex(const QString& channelName) {
    int rowIndex = 0;
    for (auto channel : channels_) {
        if (channel->getName() == channelName)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

void IrcChannelTreeModel::channelDataChanged(IrcChannel* channel) {
    auto rowIndex = getChannelIndex(channel);
    auto modelIndex = createIndex(rowIndex, 0, channel);
    emit dataChanged(modelIndex, modelIndex);
    auto server = channel->getServer().lock();
    if (!server) return;
    emit channelDataChanged(server, rowIndex);
}

void IrcChannelTreeModel::resetChannels(std::list<std::shared_ptr<IrcChannel>>& channels) {
    beginResetModel();
    channels_.clear();
    channels_.insert(channels_.begin(), channels.begin(), channels.end());
    for (auto& channel : channels) {
        emit newChannel(channel);
    }
    endResetModel();
}

void IrcChannelTreeModel::addChannel(std::shared_ptr<IrcChannel> channel) {
    int rowIndex = channels_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    auto server = channel->getServer().lock();
    emit beginInsertChannel(server, rowIndex);
    channels_.push_back(channel);
    emit newChannel(channel);
    endInsertRows();
    emit endInsertChannel();
}

void IrcChannelTreeModel::deleteChannel(const QString& channelName) {
    int rowIndex = 0;
    decltype(channels_)::iterator it;
    for (it = channels_.begin(); it != channels_.end(); ++it, ++rowIndex) {
        if ((*it)->getName() == channelName)
            break;
    }
    if (it == channels_.end()) return;
    beginRemoveRows(QModelIndex{}, rowIndex, rowIndex+1);
    auto server = (*it)->getServer().lock();
    emit beginRemoveChannel(server, rowIndex);
    channels_.erase(it);
    endRemoveRows();
    emit endRemoveChannel();
}
