#include "ChannelTreeModel.hpp"


ChannelTreeModel::ChannelTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , root_{}
{
}

QModelIndex ChannelTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    BasicTreeItem* parentItem;
    if (!parent.isValid())
        parentItem = const_cast<RootTreeItem*>(&root_);
    else
        parentItem = static_cast<BasicTreeItem*>(parent.internalPointer());

    BasicTreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ChannelTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    BasicTreeItem* childItem = static_cast<BasicTreeItem*>(index.internalPointer());
    BasicTreeItem* parentItem = childItem->parentItem();

    if (parentItem == &root_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ChannelTreeModel::rowCount(const QModelIndex& parent) const {
    BasicTreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = const_cast<RootTreeItem*>(&root_);
    else
        parentItem = static_cast<BasicTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int ChannelTreeModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return static_cast<BasicTreeItem*>(parent.internalPointer())->columnCount();
    else
        return root_.columnCount();
}

QVariant ChannelTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    BasicTreeItem* item = static_cast<BasicTreeItem*>(index.internalPointer());

    if (role == Qt::DecorationRole)
        return item->decoration(); // TODO: select icon depending on element

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

RootTreeItem* ChannelTreeModel::root() {
    return &root_;
}
