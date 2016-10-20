#include "BacklogView.hpp"

#include <QTextBlockFormat>
#include <QTextCursor>
#include <QScrollBar>


BacklogView::BacklogView(QGraphicsScene* scene)
    : QGraphicsView(scene)
    , splitting_{75, 0.2, 0.8}
{
}

void BacklogView::resizeEvent(QResizeEvent* event) {
    updateLayout();
}

void BacklogView::mouseMoveEvent(QMouseEvent* event) {
    // TODO: update cursor if over resize bar
    auto mousePosition = event->localPos();
}

void BacklogView::updateLayout() {
    auto contentsRect = this->contentsRect();
    qreal width = contentsRect.width();
    qreal timeWidth = splitting_[0]; // time is fixed width
    width -= splitting_[0];
    qreal whoWidth = splitting_[1] * width;
    qreal messageWidth = splitting_[2] * width;

    int top = 0;
    int left = 0;
    for (auto& line : chatLines_) {
        left = 0;
        auto* timestampGfx = line.getTimestampGfx();
        auto* whoGfx = line.getWhoGfx();
        auto* messageGfx = line.getMessageGfx();
        timestampGfx->setTextWidth(timeWidth);
        whoGfx->setTextWidth(whoWidth);
        messageGfx->setTextWidth(messageWidth);

        timestampGfx->setPos(left, top);
        left += timeWidth;
        whoGfx->setPos(left, top);
        left += whoWidth;
        messageGfx->setPos(left, top);

        // nick col: align right
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignRight);
        QTextCursor cursor = whoGfx->textCursor();
        cursor.select(QTextCursor::Document);
        cursor.mergeBlockFormat(format);
        cursor.clearSelection();
        whoGfx->setTextCursor(cursor);

        top += std::max({timestampGfx->boundingRect().height(), whoGfx->boundingRect().height(), messageGfx->boundingRect().height()});
    }
}

void BacklogView::addMessage(size_t id,
                             double time,
                             const QString& nick,
                             const QString& message,
                             const MessageColor color,
                             bool bUpdateLayout){
    QScrollBar* bar = this->verticalScrollBar();
    bool scrollToBottom = bar != nullptr && bar->sliderPosition() == bar->maximum();

    ChatLine* line;
    if (chatLines_.size() == 0 || id > chatLines_.back().getId()) {
        chatLines_.emplace_back(id, time, nick, message, color);
        line = &chatLines_.back();
    } else if (id < chatLines_.front().getId()) {
        chatLines_.emplace_front(id, time, nick, message, color);
        line = &chatLines_.front();
    } else {
        auto it = chatLines_.begin();
        while (id > it->getId())
            ++it;
        chatLines_.emplace(it, id, time, nick, message, color);
    }

    QGraphicsScene* scene = this->scene();
    scene->addItem(line->getTimestampGfx());
    scene->addItem(line->getWhoGfx());
    scene->addItem(line->getMessageGfx());

    if (bUpdateLayout)
        updateLayout();

    if (scrollToBottom)
        this->ensureVisible(QRectF(0, this->scene()->sceneRect().height(), 0, 0));
}
