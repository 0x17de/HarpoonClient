#ifndef BACKLOGVIEW_H
#define BACKLOGVIEW_H


#include <list>
#include <array>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>

#include "ChatLine.hpp"
#include "GraphicsHandle.hpp"


class BacklogView : public QGraphicsView {
    Q_OBJECT

    std::array<qreal, 3> splitting_;
    std::list<ChatLine> chatLines_;

    std::array<GraphicsHandle, 2> handles;

    void updateLayout(bool moveHandle1 = true, bool moveHandle2 = true);

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;

public:
    explicit BacklogView(QGraphicsScene* scene);

    void addMessage(size_t id,
                    double time,
                    const QString& nick,
                    const QString& message,
                    const MessageColor color = MessageColor::Default,
                    bool bUpdateLayout = true);
};


#endif
