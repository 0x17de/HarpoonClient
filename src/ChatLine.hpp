#ifndef CHATLINE_H
#define CHATLINE_H


#include <QString>


class ChatLine {
    QString time_;
    QString who_;
    QString message_;

public:
    ChatLine(const QString& time,
             const QString& who,
             const QString& message);

    QString getTime() const;
    QString getWho() const;
    QString getMessage() const;
};


#endif
