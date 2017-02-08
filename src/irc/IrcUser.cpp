#include "IrcUser.hpp"


IrcUser::IrcUser(const QString& nick,
           const QString& mode)
    : TreeEntry('u')
    , userGroup_{nullptr}
    , nick_(stripNick(nick))
    , mode_(mode)
{
}

QString IrcUser::stripNick(const QString& nick) {
    auto exclamationMarkPosition = nick.indexOf("!");
    return exclamationMarkPosition == -1 ? nick : nick.left(exclamationMarkPosition);
}

void IrcUser::setUserGroup(IrcUserGroup* userGroup) {
    userGroup_ = userGroup;
}

IrcUserGroup* IrcUser::getUserGroup() const {
    return userGroup_;
}

QString IrcUser::getNick() const {
    return nick_;
}

QString IrcUser::getMode() const {
    return mode_;
}

void IrcUser::rename(const QString& nick) {
    nick_ = nick;
}

void IrcUser::changeMode(char modeChar, bool add) {
    if (add) {
        if (!mode_.contains(modeChar))
            mode_.append(modeChar);
    } else {
        mode_.remove(modeChar);
    }
}

char IrcUser::getAccessMode() {
    // to keep order
    return isOwner() ? 'q' : isAdmin() ? 'a' : isOperator() ? 'o' : isHalfOperator() ? 'h' : isVoiced() ? 'v' : 0;
}

bool IrcUser::isOwner() {
    return mode_.contains('q');
}

bool IrcUser::isAdmin() {
    return mode_.contains('a');
}

bool IrcUser::isOperator() {
    return mode_.contains('o');
}

bool IrcUser::isHalfOperator() {
    return mode_.contains('h');
}

bool IrcUser::isVoiced() {
    return mode_.contains('v');
}
