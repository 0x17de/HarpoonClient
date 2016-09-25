#ifndef USER_H
#define USER_H


#include <QString>

#include "TreeEntry.hpp"


class UserGroup;
class User : public TreeEntry {
    UserGroup* userGroup_;
    QString nick_;
public:
    User(const QString& nick);

    static QString stripNick(const QString& nick);
    void setUserGroup(UserGroup* userGroup);
    UserGroup* getUserGroup() const;
    QString getNick() const;
    void rename(const QString& newNick);
};


#endif
