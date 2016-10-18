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
    resizeLines();
}

void BacklogView::mouseMoveEvent(QMouseEvent* event) {
    // TODO: update cursor if over resize bar
    auto mousePosition = event->localPos();
}

void BacklogView::resizeLines() {
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
        auto* timeGfx = line.getTimeGfx();
        auto* whoGfx = line.getWhoGfx();
        auto* messageGfx = line.getMessageGfx();
        timeGfx->setTextWidth(timeWidth);
        whoGfx->setTextWidth(whoWidth);
        messageGfx->setTextWidth(messageWidth);

        timeGfx->setPos(left, top);
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

        top += std::max({timeGfx->boundingRect().height(), whoGfx->boundingRect().height(), messageGfx->boundingRect().height()});
    }
}

void BacklogView::addMessage(const QString& time,
                             const QString& nick,
                             const QString& message,
                             const MessageColor color){
    QScrollBar* bar = this->verticalScrollBar();
    bool scrollToBottom = bar != nullptr && bar->sliderPosition() == bar->maximum();

    chatLines_.emplace_back(time, nick, message, color);
    ChatLine& line = chatLines_.back();

    QGraphicsScene* scene = this->scene();
    scene->addItem(line.getTimeGfx());
    scene->addItem(line.getWhoGfx());
    scene->addItem(line.getMessageGfx());

    resizeLines();

    if (scrollToBottom)
        this->ensureVisible(QRectF(0, this->scene()->sceneRect().height(), 0, 0));
}
