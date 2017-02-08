#include "IrcChatLine.hpp"

#include <QDateTime>
#include <QTime>


IrcChatLine::IrcChatLine(size_t id,
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

QString IrcChatLine::formatTimestamp(double timestamp) {
    return QTime{QDateTime{QDateTime::fromTime_t(timestamp/1000)}.time()}.toString("[hh:mm:ss]");
}

size_t IrcChatLine::getId() const {
    return id_;
}

double IrcChatLine::getTime() const {
    return time_;
}

QString IrcChatLine::getTimestamp() const {
    return timestamp_;
}

QString IrcChatLine::getWho() const {
    return who_;
}

QString IrcChatLine::getMessage() const {
    return message_;
}

const QString& IrcChatLine::getTimestampRef() const {
    return timestamp_;
}

const QString& IrcChatLine::getWhoRef() const {
    return who_;
}

const QString& IrcChatLine::getMessageRef() const {
    return message_;
}

QGraphicsTextItem* IrcChatLine::getTimestampGfx() {
    return &timestampGfx_;
}

QGraphicsTextItem* IrcChatLine::getWhoGfx() {
    return &whoGfx_;
}

QGraphicsTextItem* IrcChatLine::getMessageGfx() {
    return &messageGfx_;
}
