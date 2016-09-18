#include "UserGroup.hpp"
#include "User.hpp"


UserGroup::UserGroup(const QString& name)
    : TreeEntry('g')
    , name_{name}
    , expanded_{true}
{
}

void UserGroup::addUser(std::shared_ptr<User> user) {
    users_.push_back(user);
    user->setUserGroup(this);
}

int UserGroup::getUserCount() const {
    return users_.size();
}

int UserGroup::getUserIndex(User* user) const {
    int rowIndex = 0;
    for (auto u : users_) {
        if (u.get() == user)
            return rowIndex;
        ++rowIndex;
    }
    return -1;
}

User* UserGroup::getUser(QString nick) {
    auto it = std::find_if(users_.begin(), users_.end(), [&nick](const std::shared_ptr<User>& user){
            return user->getNick() == nick;
        });
    return (it == users_.end() ? nullptr : (*it).get());
}

User* UserGroup::getUser(int position) {
    auto it = users_.begin();
    std::advance(it, position);
    return (*it).get();
}

QString UserGroup::getName() const {
    return name_;
}

bool UserGroup::getExpanded() const {
    return expanded_;
}
