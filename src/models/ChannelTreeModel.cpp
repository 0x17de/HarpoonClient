#include "ChannelTreeModel.hpp"
#include "../Server.hpp"
#include "../Channel.hpp"

#include <QIcon>


ChannelTreeModel::ChannelTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex ChannelTreeModel::index(int row, int column, const QModelIndex& parent) const {
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

QModelIndex ChannelTreeModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int ChannelTreeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return channels_.size();
    } else {
        auto* item = static_cast<TreeEntry*>(parent.internalPointer());
        if (item->getTreeEntryType() == 's') {
            Server* server = static_cast<Server*>(parent.internalPointer());
            return server->getChannelModel().rowCount();
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
            return QIcon(channel->getDisabled() ? ":icons/channelDisabled.png" : ":icons/channel.png");

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

std::list<std::shared_ptr<Channel>> ChannelTreeModel::getChannels() {
    return channels_;
}

Channel* ChannelTreeModel::getChannel(const QString& channelName) {
    auto it = find_if(channels_.begin(), channels_.end(), [&channelName](std::shared_ptr<Channel> channel) {
            return channel->getName() == channelName;
        });
    if (it == channels_.end()) return nullptr;
    return it->get();
}

Channel* ChannelTreeModel::getChannel(int row) {
    return static_cast<Channel*>(index(row, 0).internalPointer());
}

int ChannelTreeModel::getChannelIndex(Channel* channel) {
    int rowIndex = 0;
    for (auto s : channels_) {
        if (s.get() == channel)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

int ChannelTreeModel::getChannelIndex(const QString& channelName) {
    int rowIndex = 0;
    for (auto channel : channels_) {
        if (channel->getName() == channelName)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

void ChannelTreeModel::channelDataChanged(Channel* channel) {
    auto rowIndex = getChannelIndex(channel);
    auto modelIndex = createIndex(rowIndex, 0, channel);
    emit dataChanged(modelIndex, modelIndex);
}

void ChannelTreeModel::resetChannels(std::list<std::shared_ptr<Channel>>& channels) {
    beginResetModel();
    channels_.clear();
    channels_.insert(channels_.begin(), channels.begin(), channels.end());
    endResetModel();
}

void ChannelTreeModel::newChannel(std::shared_ptr<Channel> channel) {
    int rowIndex = channels_.size();
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    channels_.push_back(channel);
    endInsertRows();
}

void ChannelTreeModel::deleteChannel(const QString& channelName) {
    int rowIndex = 0;
    decltype(channels_)::iterator it;
    for (it = channels_.begin(); it != channels_.end(); ++it, ++rowIndex) {
        if ((*it)->getName() == channelName)
            break;
    }
    if (it == channels_.end()) return;
    beginRemoveRows(QModelIndex{}, rowIndex, rowIndex+1);
    channels_.erase(it);
    endRemoveRows();
}
