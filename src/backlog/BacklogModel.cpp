#include "BacklogModel.hpp"


BacklogModel::BacklogModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex BacklogModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= messages_.size())
            return QModelIndex();
        auto it = messages_.begin();
        std::advance(it, row);

        const ChatLine& line = *it;
        const QString* data;
        if (column == 0) {
            data = &line.getTimeRef();
        } else if (column == 1) {
            data = &line.getWhoRef();
        } else if (column == 2) {
            data = &line.getMessageRef();
        }
        return createIndex(row, column, (void*)data);
    }
    return QModelIndex();
}

QModelIndex BacklogModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    return QModelIndex();
}

int BacklogModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return messages_.size();

    return 0;
}

int BacklogModel::columnCount(const QModelIndex& parent) const {
    return 3; // only one column for all data
}

QVariant BacklogModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    auto* ptr = index.internalPointer();
    auto* item = static_cast<QString*>(ptr);
    return *item;
}

Qt::ItemFlags BacklogModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant BacklogModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return "Time";
        if (section == 1)
            return "Nick";
        return "Message";
    }

    return QVariant();
}

void BacklogModel::addMessage(const QString& time,
                              const QString& nick,
                              const QString& message) {
    int rowIndex = messages_.size();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    messages_.push_back({time, nick, message});
    endInsertRows();
}
