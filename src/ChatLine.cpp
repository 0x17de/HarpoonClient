#include "ChatLine.hpp"


ChatLine::ChatLine(const QString& time,
                   const QString& who,
                   const QString& message)
    : time_{time}
    , who_{who}
    , message_{message}
{
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