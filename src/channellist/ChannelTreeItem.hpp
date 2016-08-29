#ifndef CHANNELTREEITEM_H
#define CHANNELTREEITEM_H

#include <memory>
#include <QList>
#include <QString>
#include <QVariant>

class ChannelTreeModel; // debug
class BasicTreeItem : public std::enable_shared_from_this<BasicTreeItem> {
    QList<std::shared_ptr<BasicTreeItem>> childItems_;
protected:
    QString data_;
    BasicTreeItem* parent_;
public:
    explicit BasicTreeItem(const QString& name, BasicTreeItem* parent = 0);

    void appendChild(std::shared_ptr<BasicTreeItem> child);

    BasicTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QString data(int column) const;
    int row() const;
    BasicTreeItem* parentItem();

    virtual QVariant decoration();
    friend ChannelTreeModel; // debug
};

class ServerTreeItem;
class ChannelTreeItem;
class RootTreeItem : public BasicTreeItem {
public:
    RootTreeItem();
    ServerTreeItem* addServer(const QString& name);
};

class ServerTreeItem : public BasicTreeItem {
public:
    ServerTreeItem(const QString& name, RootTreeItem* root);
    ChannelTreeItem* addChannel(const QString& name, bool isUser = false);
    virtual QVariant decoration() override;
};

class ChannelTreeItem : public BasicTreeItem {
    bool isUser;
public:
    ChannelTreeItem(const QString& name, bool isUser, ServerTreeItem* root);
    virtual QVariant decoration() override;
};

#endif
