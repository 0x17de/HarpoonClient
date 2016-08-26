#include "ChannelTreeItem.hpp"


ChannelTreeItem::ChannelTreeItem(ChannelTreeItem* parent)
    : parent_{parent}
    , data_{"TEST"}
{
}

void ChannelTreeItem::appendChild(std::shared_ptr<ChannelTreeItem> item) {
    childItems_.append(item);
}

ChannelTreeItem* ChannelTreeItem::child(int row) {
    return childItems_.value(row).get();
}

int ChannelTreeItem::childCount() const {
    return childItems_.count();
}

int ChannelTreeItem::row() const {
    if (parent_)
        return parent_->childItems_.indexOf(std::const_pointer_cast<ChannelTreeItem>(shared_from_this()));
    return 0;
}

int ChannelTreeItem::columnCount() const {
    return 1;
}

QString ChannelTreeItem::data(int column) const {
    return data_;
}

ChannelTreeItem* ChannelTreeItem::parentItem() {
    return parent_;
}
