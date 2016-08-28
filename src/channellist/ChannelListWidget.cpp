#include "ChannelListWidget.hpp"
#include "ChannelTreeModel.hpp"


ChannelListWidget::ChannelListWidget(QWidget* parent)
    : QTreeView(parent)
{
    setModel(model_ = new ChannelTreeModel());
}

ChannelTreeItem* ChannelListWidget::addServer(const QString& name) {
    return model_->addServer(name);
}

void ChannelListWidget::addChannel(ChannelTreeItem* server, const QString& name) {
    model_->addChannel(server, name);
}
