#include "IrcUserGroup.hpp"
#include "IrcUser.hpp"


IrcUserGroup::IrcUserGroup(const QString& name)
    : TreeEntry('g')
    , name_{name}
    , expanded_{true}
{
}

void IrcUserGroup::addUser(std::shared_ptr<IrcUser> user) {
    users_.push_back(user);
    user->setUserGroup(this);
}

void IrcUserGroup::removeUser(IrcUser* user) {
    users_.remove_if([user](const std::shared_ptr<IrcUser>& userPtr) {
            return user == userPtr.get();
        });
}

int IrcUserGroup::getUserCount() const {
    return users_.size();
}

int IrcUserGroup::getUserIndex(IrcUser* user) const {
    int rowIndex = 0;
    for (auto u : users_) {
        if (u.get() == user)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

IrcUser* IrcUserGroup::getUser(QString nick) {
    auto it = std::find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<IrcUser>& user){
            return user->getNick() == nick;
        });
    return (it == users_.end() ? nullptr : (*it).get());
}

IrcUser* IrcUserGroup::getUser(int position) {
    auto it = users_.begin();
    std::advance(it, position);
    return (*it).get();
}

QString IrcUserGroup::getName() const {
    return name_;
}

bool IrcUserGroup::getExpanded() const {
    return expanded_;
}
