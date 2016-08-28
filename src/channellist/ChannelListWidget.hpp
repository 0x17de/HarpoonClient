#ifndef CHANNELLISTWIDGET_H
#define CHANNELLISTWIDGET_H


#include <QTreeView>


class ChannelTreeItem;
class ChannelTreeModel;
class ChannelListWidget : public QTreeView {
    ChannelTreeModel* model_;
public:
    ChannelListWidget(QWidget* parent);
    ChannelTreeItem* addServer(const QString& name);
    void addChannel(ChannelTreeItem* server, const QString& name);
};


#endif
