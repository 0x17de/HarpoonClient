#ifndef IRCBACKLOGVIEW_H
#define IRCBACKLOGVIEW_H


#include <list>
#include <array>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>

#include "irc/IrcChatLine.hpp"
#include "GraphicsHandle.hpp"


class IrcBacklogView : public QGraphicsView {
    Q_OBJECT

    std::array<qreal, 3> splitting_;
    std::list<IrcChatLine> chatLines_;

    std::array<GraphicsHandle, 2> handles;

    void updateLayout(bool moveHandle1 = true, bool moveHandle2 = true);

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;

public:
    explicit IrcBacklogView(QGraphicsScene* scene);

    void addMessage(size_t id,
                    double time,
                    const QString& nick,
                    const QString& message,
                    const MessageColor color = MessageColor::Default,
                    bool bUpdateLayout = true);
};


#endif
