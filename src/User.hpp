#ifndef USER_H
#define USER_H


#include <QString>

#include "TreeEntry.hpp"


class UserGroup;
class User : public TreeEntry {
    UserGroup* userGroup_;
    QString nick_;
    QString mode_;
public:
    explicit User(const QString& nick,
                  const QString& mode = "");

    static QString stripNick(const QString& nick);
    void setUserGroup(UserGroup* userGroup);
    UserGroup* getUserGroup() const;
    QString getNick() const;
    QString getMode() const;
    void rename(const QString& newNick);

    char getAccessMode();
    bool isOwner();
    bool isAdmin();
    bool isOperator();
    bool isHalfOperator();
    bool isVoiced();
};


#endif
