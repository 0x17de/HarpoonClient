#include "IrcBacklogView.hpp"
#include "moc_IrcBacklogView.cpp"

#include <QTextBlockFormat>
#include <QTextCursor>
#include <QScrollBar>


IrcBacklogView::IrcBacklogView(QGraphicsScene* scene)
    : QGraphicsView(scene)
    , splitting_{75, 0.2, 0.8}
{
    for (auto& handle : handles)
        scene->addItem(&handle);

    connect(&handles[0], &GraphicsHandle::positionChanged, [this](qreal xpos) {
            splitting_[0] = xpos;
            updateLayout(false, true);
        });
    connect(&handles[1], &GraphicsHandle::positionChanged, [this](qreal xpos) {
            auto contentsRect = this->contentsRect();
            qreal width = contentsRect.width();
            qreal timeWidth = splitting_[0]; // time is fixed width
            width -= splitting_[0];
            qreal whoWidth = splitting_[1] * width;
            qreal messageWidth = splitting_[2] * width;
            qreal pos = xpos - timeWidth;
            qreal split = pos / (whoWidth + messageWidth);

            splitting_[1] = split;
            splitting_[2] = 1.0 - split;
            updateLayout(false, false);
        });

    setAcceptDrops(true);
}

void IrcBacklogView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    updateLayout();
}

void IrcBacklogView::mousePressEvent(QMouseEvent* event) {
    QGraphicsView::mousePressEvent(event);
}

void IrcBacklogView::updateLayout(bool moveHandle1, bool moveHandle2) {
    auto contentsRect = this->contentsRect();
    qreal width = contentsRect.width();
    qreal timeWidth = splitting_[0]; // time is fixed width
    width -= splitting_[0];
    qreal whoWidth = splitting_[1] * width;
    qreal messageWidth = splitting_[2] * width;

    int top = 0;
    for (auto& line : chatLines_) {
        int left = 0;
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

    qreal height = std::max(top, viewport()->height());
    if (moveHandle1) {
        handles[0].setRect(QRect(-GraphicsHandle::handleWidth/2, 0, GraphicsHandle::handleWidth/2, height));
        handles[0].setPos(timeWidth, 0);
    }
    if (moveHandle2) {
        handles[1].setRect(QRect(-GraphicsHandle::handleWidth/2, 0, GraphicsHandle::handleWidth/2, height));
        handles[1].setPos(timeWidth+whoWidth, 0);
    }
}

void IrcBacklogView::addMessage(size_t id,
                             double time,
                             const QString& nick,
                             const QString& message,
                             const MessageColor color,
                             bool bUpdateLayout){
    QScrollBar* bar = this->verticalScrollBar();
    bool scrollToBottom = bar != nullptr && bar->sliderPosition() == bar->maximum();

    IrcChatLine* line;
    if (id == 0 || chatLines_.size() == 0 || id > chatLines_.back().getId()) {
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
