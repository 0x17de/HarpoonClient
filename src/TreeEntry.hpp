#ifndef TREEENTRY_H
#define TREEENTRY_H

#include <QObject>
#include <memory>


class TreeEntry : public QObject, public std::enable_shared_from_this<TreeEntry> {
    char entryType;
public:
    explicit TreeEntry(char entryType);
    char getTreeEntryType() const;
};


#endif
