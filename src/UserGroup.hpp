#ifndef USERGROUP_H
#define USERGROUP_H


#include <memory>
#include <list>
#include <QString>

#include "TreeEntry.hpp"


class User;
class UserGroup : public TreeEntry {
    std::list<std::shared_ptr<User>> users_;
    QString name_;
    bool expanded_;
public:
    UserGroup(const QString& name);

    void addUser(std::shared_ptr<User> channel);
    void removeUser(User* user);
    int getUserCount() const;
    int getUserIndex(User* user) const;
    User* getUser(QString user);
    User* getUser(int position);
    QString getName() const;
    bool getExpanded() const;
};


#endif
