#ifndef CHANNELLISTWIDGET_H
#define CHANNELLISTWIDGET_H


#include <QTreeView>


class RootTreeItem;
class ChannelTreeModel;
class ChannelListWidget : public QTreeView {
    ChannelTreeModel* model_;
public:
    ChannelListWidget(QWidget* parent);
    RootTreeItem* getRoot();
};


#endif
