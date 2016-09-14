#include "User.hpp"


User::User(const QString& name)
    : TreeEntry('u')
    , userGroup_{0}
    , name_{name}
{
}

void User::setUserGroup(UserGroup* userGroup) {
    userGroup_ = userGroup;
}

UserGroup* User::getUserGroup() const {
    return userGroup_;
}

QString User::getName() const {
    return name_;
}
