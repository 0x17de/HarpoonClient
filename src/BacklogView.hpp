#ifndef BACKLOGVIEW_H
#define BACKLOGVIEW_H


#include <list>
#include <array>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>

#include "ChatLine.hpp"


class BacklogView : public QGraphicsView {
    Q_OBJECT

    std::array<qreal, 3> splitting_;
    std::list<ChatLine> chatLines_;

    void updateLayout();

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

public:
    BacklogView(QGraphicsScene* scene);

    void addMessage(size_t id,
                    double time,
                    const QString& nick,
                    const QString& message,
                    const MessageColor color = MessageColor::Default,
                    bool bUpdateLayout = true);
};


#endif
