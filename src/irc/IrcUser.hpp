#ifndef USERIRC_H
#define USERIRC_H


#include <QString>

#include "TreeEntry.hpp"


class IrcUserGroup;
class IrcUser : public TreeEntry {
    IrcUserGroup* userGroup_;
    QString nick_;
    QString mode_;
public:
    explicit IrcUser(const QString& nick,
                  const QString& mode = "");

    static QString stripNick(const QString& nick);
    void setUserGroup(IrcUserGroup* userGroup);
    IrcUserGroup* getUserGroup() const;
    QString getNick() const;
    QString getMode() const;
    void rename(const QString& newNick);
    void changeMode(char modeChar, bool add);

    char getAccessMode();
    bool isOwner();
    bool isAdmin();
    bool isOperator();
    bool isHalfOperator();
    bool isVoiced();
};


#endif
