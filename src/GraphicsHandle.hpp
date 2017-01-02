#ifndef GRAPHICSHANDLE_H
#define GRAPHICSHANDLE_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneHoverEvent>


class GraphicsHandle : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    constexpr static int handleWidth = 10;

    GraphicsHandle();

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void positionChanged(qreal xpos);
};

#endif
