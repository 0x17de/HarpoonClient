#include "User.hpp"


User::User(const QString& nick,
           const QString& mode)
    : TreeEntry('u')
    , userGroup_{nullptr}
    , nick_(stripNick(nick))
    , mode_(mode)
{
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

QString User::getMode() const {
    return mode_;
}

void User::rename(const QString& nick) {
    nick_ = nick;
}

char User::getAccessMode() {
    // to keep order
    return isOwner() ? 'q' : isAdmin() ? 'a' : isOperator() ? 'o' : isHalfOperator() ? 'h' : isVoiced() ? 'v' : 0;
}

bool User::isOwner() {
    return mode_.contains('q');
}

bool User::isAdmin() {
    return mode_.contains('a');
}

bool User::isOperator() {
    return mode_.contains('o');
}

bool User::isHalfOperator() {
    return mode_.contains('h');
}

bool User::isVoiced() {
    return mode_.contains('v');
}
