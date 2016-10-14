#include "ChatLine.hpp"


ChatLine::ChatLine(const QString& time,
                   const QString& who,
                   const QString& message,
                   const MessageColor color)
    : time_{time}
    , who_{who}
    , message_{message}
    , timeGfx_(time)
    , whoGfx_(who)
    , messageGfx_(message)
{
    switch (color) {
    case MessageColor::Notice:
        timeGfx_.setDefaultTextColor(Qt::darkYellow);
        whoGfx_.setDefaultTextColor(Qt::darkYellow);
        messageGfx_.setDefaultTextColor(Qt::darkYellow);
        break;
    case MessageColor::Event:
        timeGfx_.setDefaultTextColor(Qt::darkMagenta);
        whoGfx_.setDefaultTextColor(Qt::darkMagenta);
        messageGfx_.setDefaultTextColor(Qt::darkMagenta);
        break;
    case MessageColor::Action:
        timeGfx_.setDefaultTextColor(Qt::darkBlue);
        whoGfx_.setDefaultTextColor(Qt::darkBlue);
        messageGfx_.setDefaultTextColor(Qt::darkBlue);
        break;
    }
}

QString ChatLine::getTime() const {
    return time_;
}

QString ChatLine::getWho() const {
    return who_;
}

QString ChatLine::getMessage() const {
    return message_;
}

const QString& ChatLine::getTimeRef() const {
    return time_;
}

const QString& ChatLine::getWhoRef() const {
    return who_;
}

const QString& ChatLine::getMessageRef() const {
    return message_;
}

QGraphicsTextItem* ChatLine::getTimeGfx() {
    return &timeGfx_;
}

QGraphicsTextItem* ChatLine::getWhoGfx() {
    return &whoGfx_;
}

QGraphicsTextItem* ChatLine::getMessageGfx() {
    return &messageGfx_;
}
