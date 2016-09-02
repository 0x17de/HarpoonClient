#include "TreeEntry.hpp"


TreeEntry::TreeEntry(char entryType)
    : entryType{entryType}
{
}

char TreeEntry::getTreeEntryType() const {
    return entryType;
}
