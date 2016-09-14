#ifndef USER_H
#define USER_H


#include <QString>

#include "TreeEntry.hpp"


class UserGroup;
class User : public TreeEntry {
    UserGroup* userGroup_;
    QString name_;
public:
    User(const QString& name);

    void setUserGroup(UserGroup* userGroup);
    UserGroup* getUserGroup() const;
    QString getName() const;
};


#endif
