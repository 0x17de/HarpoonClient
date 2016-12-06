#include "SettingsTypeModel.hpp"


SettingsTypeModel::SettingsTypeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex SettingsTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    std::list<QString>& entries = const_cast<std::list<QString>&>(typeNames_);

    if (!parent.isValid()) {
        if (row >= entries.size())
            return QModelIndex();
        auto it = entries.begin();
        std::advance(it, row);
        return createIndex(row, column, &(*it));
    }
    return QModelIndex();
}

QModelIndex SettingsTypeModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int SettingsTypeModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return typeNames_.size();

    return 0;
}

int SettingsTypeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant SettingsTypeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto* item = static_cast<QString*>(index.internalPointer());

    if (role == Qt::DecorationRole)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return *item;
}

Qt::ItemFlags SettingsTypeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant SettingsTypeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Users";

    return QVariant();
}

void SettingsTypeModel::newType(const QString& name) {
    int rowIndex = 0;
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    typeNames_.push_back(name);
    endInsertRows();
}

void SettingsTypeModel::resetTypes(const std::list<QString>& types) {
    beginResetModel();
    typeNames_.clear();
    for (auto& name : types)
        typeNames_.push_back(name);
    endResetModel();
}
