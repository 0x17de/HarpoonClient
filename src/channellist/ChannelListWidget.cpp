#include "ChannelListWidget.hpp"
#include "ChannelTreeModel.hpp"


ChannelListWidget::ChannelListWidget(QWidget* parent)
    : QTreeView(parent)
{
    setModel(new ChannelTreeModel());
}
