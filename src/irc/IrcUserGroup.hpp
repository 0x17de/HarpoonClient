#ifndef USERGROUPIRC_H
#define USERGROUPIRC_H


#include <memory>
#include <list>
#include <QString>

#include "TreeEntry.hpp"


class IrcUser;
class IrcUserGroup : public TreeEntry {
    std::list<std::shared_ptr<IrcUser>> users_;
    QString name_;
    bool expanded_;
public:
    explicit IrcUserGroup(const QString& name);

    void addUser(std::shared_ptr<IrcUser> channel);
    void removeUser(IrcUser* user);
    int getUserCount() const;
    int getUserIndex(IrcUser* user) const;
    IrcUser* getUser(QString user);
    IrcUser* getUser(int position);
    QString getName() const;
    bool getExpanded() const;
};


#endif
