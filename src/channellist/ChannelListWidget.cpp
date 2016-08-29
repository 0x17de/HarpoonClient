#include "ChannelListWidget.hpp"
#include "ChannelTreeModel.hpp"


ChannelListWidget::ChannelListWidget(QWidget* parent)
    : QTreeView(parent)
{
    setModel(model_ = new ChannelTreeModel());
}

RootTreeItem* ChannelListWidget::getRoot() {
    return model_->root();
}
