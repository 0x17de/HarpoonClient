#include "ChannelTreeModel.hpp"

#include <QIcon>


ChannelTreeModel::ChannelTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex ChannelTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ChannelTreeItem* parentItem;
    if (!parent.isValid())
        parentItem = const_cast<ChannelTreeItem*>(&root_);
    else
        parentItem = static_cast<ChannelTreeItem*>(parent.internalPointer());

    ChannelTreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ChannelTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    ChannelTreeItem* childItem = static_cast<ChannelTreeItem*>(index.internalPointer());
    ChannelTreeItem* parentItem = childItem->parentItem();

    if (parentItem == &root_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ChannelTreeModel::rowCount(const QModelIndex& parent) const {
    ChannelTreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = const_cast<ChannelTreeItem*>(&root_);
    else
        parentItem = static_cast<ChannelTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int ChannelTreeModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return static_cast<ChannelTreeItem*>(parent.internalPointer())->columnCount();
    else
        return root_.columnCount();
}

QVariant ChannelTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    ChannelTreeItem* item = static_cast<ChannelTreeItem*>(index.internalPointer());

    if (role == Qt::DecorationRole)
        return QIcon(":/icons/channel.png"); // TODO: select icon depending on element

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->data(index.column());
}

Qt::ItemFlags ChannelTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant ChannelTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root_.data(section);

    return QVariant();
}

