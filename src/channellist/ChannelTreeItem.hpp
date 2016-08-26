#ifndef CHANNELTREEITEM_H
#define CHANNELTREEITEM_H

#include <memory>
#include <QList>
#include <QString>


class ChannelTreeItem : public std::enable_shared_from_this<ChannelTreeItem> {
    QList<std::shared_ptr<ChannelTreeItem>> childItems_;
    QString data_;
    ChannelTreeItem* parent_;
public:
    explicit ChannelTreeItem(ChannelTreeItem* parent = 0);

    void appendChild(std::shared_ptr<ChannelTreeItem> child);

    ChannelTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QString data(int column) const;
    int row() const;
    ChannelTreeItem* parentItem();
};

#endif
