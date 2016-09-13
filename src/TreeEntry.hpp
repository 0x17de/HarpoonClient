#ifndef TREEENTRY_H
#define TREEENTRY_H

#include <QObject>


class TreeEntry : public QObject {
    char entryType;
public:
    TreeEntry(char entryType);
    char getTreeEntryType() const;
};


#endif
