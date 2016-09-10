#ifndef USER_H
#define USER_H


#include <QString>

#include "TreeEntry.hpp"


class UserGroup;
class User : public TreeEntry {
    UserGroup* userGroup_;
    QString name_;
public:
    User(UserGroup* server, const QString& name);

    UserGroup* getUserGroup() const;
    QString getName() const;
};


#endif
