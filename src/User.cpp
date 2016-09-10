#include "User.hpp"


User::User(UserGroup* userGroup, const QString& name)
    : TreeEntry('u')
    , userGroup_{userGroup}
    , name_{name}
{
}

UserGroup* User::getUserGroup() const {
    return userGroup_;
}

QString User::getName() const {
    return name_;
}
