#include "GraphicsHandle.hpp"
#include "moc_GraphicsHandle.cpp"
#include <QPen>
#include <QColor>


constexpr int GraphicsHandle::handleWidth;

GraphicsHandle::GraphicsHandle()
    : QGraphicsRectItem()
{
    setPen(QPen(Qt::NoPen));
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setZValue(50);
}

void GraphicsHandle::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsRectItem::hoverEnterEvent(event);
    setBrush(QBrush(QColor("lightgrey")));
}

void GraphicsHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsRectItem::hoverLeaveEvent(event);
    setBrush(QBrush(Qt::NoBrush));
}

QVariant GraphicsHandle::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        newPos.setY(0);
        emit positionChanged(newPos.x());
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}
