#include "User.hpp"


User::User(const QString& nick)
    : TreeEntry('u')
    , userGroup_{nullptr}
{
    nick_ = stripNick(nick);
}

QString User::stripNick(const QString& nick) {
    auto exclamationMarkPosition = nick.indexOf("!");
    return exclamationMarkPosition == -1 ? nick : nick.left(exclamationMarkPosition);
}

void User::setUserGroup(UserGroup* userGroup) {
    userGroup_ = userGroup;
}

UserGroup* User::getUserGroup() const {
    return userGroup_;
}

QString User::getNick() const {
    return nick_;
}

void User::rename(const QString& nick) {
    nick_ = nick;
}
