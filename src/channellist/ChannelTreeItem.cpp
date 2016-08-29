#include "ChannelTreeItem.hpp"

#include <QIcon>


BasicTreeItem::BasicTreeItem(const QString& name, BasicTreeItem* parent)
    : parent_{parent}
    , data_{name}
{
}

void BasicTreeItem::appendChild(std::shared_ptr<BasicTreeItem> item) {
    childItems_.append(item);
}

BasicTreeItem* BasicTreeItem::child(int row) {
    return childItems_.value(row).get();
}

int BasicTreeItem::childCount() const {
    return childItems_.count();
}

int BasicTreeItem::row() const {
    if (parent_)
        return parent_->childItems_.indexOf(std::const_pointer_cast<BasicTreeItem>(shared_from_this()));
    return 0;
}

int BasicTreeItem::columnCount() const {
    return 1;
}

QString BasicTreeItem::data(int column) const {
    return data_;
}

BasicTreeItem* BasicTreeItem::parentItem() {
    return parent_;
}

QVariant BasicTreeItem::decoration() {
    return QVariant();
}

RootTreeItem::RootTreeItem()
    : BasicTreeItem("", 0)
{
}

ServerTreeItem::ServerTreeItem(const QString& name, RootTreeItem* root)
    : BasicTreeItem(name, root)
{
}

QVariant ServerTreeItem::decoration() {
    return QIcon(":/icons/channel.png");
}

ChannelTreeItem::ChannelTreeItem(const QString& name, bool isUser, ServerTreeItem* server)
    : BasicTreeItem(name, server)
    , isUser{isUser}
{
}

QVariant ChannelTreeItem::decoration(){
    return QIcon(isUser ? ":/icons/user.png" : ":/icons/channel.png");
}

ServerTreeItem* RootTreeItem::addServer(const QString& name) {
    auto server = std::make_shared<ServerTreeItem>(name, this);
    appendChild(server);
    return server.get();
}

ChannelTreeItem* ServerTreeItem::addChannel(const QString& name, bool isUser) {
    auto channel = std::make_shared<ChannelTreeItem>(name, isUser, this);
    appendChild(channel);
}
