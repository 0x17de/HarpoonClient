#include "ChatLine.hpp"

#include <QDateTime>
#include <QTime>


ChatLine::ChatLine(size_t id,
                   double time,
                   const QString& who,
                   const QString& message,
                   const MessageColor color)
    : id_{id}
    , time_{time}
    , timestamp_{formatTimestamp(time)}
    , who_{who}
    , message_{message}
    , timestampGfx_(timestamp_)
    , whoGfx_(who)
    , messageGfx_(message)
{
    switch (color) {
    case MessageColor::Notice:
        timestampGfx_.setDefaultTextColor(Qt::darkYellow);
        whoGfx_.setDefaultTextColor(Qt::darkYellow);
        messageGfx_.setDefaultTextColor(Qt::darkYellow);
        break;
    case MessageColor::Event:
        timestampGfx_.setDefaultTextColor(Qt::darkMagenta);
        whoGfx_.setDefaultTextColor(Qt::darkMagenta);
        messageGfx_.setDefaultTextColor(Qt::darkMagenta);
        break;
    case MessageColor::Action:
        timestampGfx_.setDefaultTextColor(Qt::darkBlue);
        whoGfx_.setDefaultTextColor(Qt::darkBlue);
        messageGfx_.setDefaultTextColor(Qt::darkBlue);
        break;
    }
}

QString ChatLine::formatTimestamp(double timestamp) {
    return QTime{QDateTime{QDateTime::fromTime_t(timestamp/1000)}.time()}.toString("[hh:mm:ss]");
}

size_t ChatLine::getId() const {
    return id_;
}

double ChatLine::getTime() const {
    return time_;
}

QString ChatLine::getTimestamp() const {
    return timestamp_;
}

QString ChatLine::getWho() const {
    return who_;
}

QString ChatLine::getMessage() const {
    return message_;
}

const QString& ChatLine::getTimestampRef() const {
    return timestamp_;
}

const QString& ChatLine::getWhoRef() const {
    return who_;
}

const QString& ChatLine::getMessageRef() const {
    return message_;
}

QGraphicsTextItem* ChatLine::getTimestampGfx() {
    return &timestampGfx_;
}

QGraphicsTextItem* ChatLine::getWhoGfx() {
    return &whoGfx_;
}

QGraphicsTextItem* ChatLine::getMessageGfx() {
    return &messageGfx_;
}
