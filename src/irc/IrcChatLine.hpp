#ifndef CHATLINEIRC_H
#define CHATLINEIRC_H


#include <QString>
#include <QGraphicsTextItem>


enum class MessageColor {
    Default,
    Notice,
    Event,
    Action
};

class IrcChatLine {
    size_t id_;
    double time_;
    QString timestamp_;
    QString who_;
    QString message_;
    QGraphicsTextItem timestampGfx_;
    QGraphicsTextItem whoGfx_;
    QGraphicsTextItem messageGfx_;

    static QString formatTimestamp(double timestamp);

public:
    IrcChatLine(size_t id,
             double time,
             const QString& who,
             const QString& message,
             const MessageColor color = MessageColor::Default);

    size_t getId() const;
    double getTime() const;
    QString getTimestamp() const;
    QString getWho() const;
    QString getMessage() const;
    const QString& getTimestampRef() const;
    const QString& getWhoRef() const;
    const QString& getMessageRef() const;
    QGraphicsTextItem* getTimestampGfx();
    QGraphicsTextItem* getWhoGfx();
    QGraphicsTextItem* getMessageGfx();
};


#endif
