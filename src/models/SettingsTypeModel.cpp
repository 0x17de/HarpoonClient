#include "SettingsTypeModel.hpp"


SettingsTypeModel::SettingsTypeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex SettingsTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= settingsWidgets_.size())
            return QModelIndex();
        auto it = settingsWidgets_.begin();
        std::advance(it, row);
        return createIndex(row, column, &it);
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
        return settingsWidgets_.size();

    return 0;
}

int SettingsTypeModel::columnCount(const QModelIndex& parent) const {
    return 1; // only one column for all data
}

QVariant SettingsTypeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto* item = static_cast<Entry*>(index.internalPointer());

    if (role == Qt::DecorationRole)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->name;
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

void SettingsTypeModel::newType(const QString& name, QWidget* widget) {
    int rowIndex = 0;
    beginInsertRows(QModelIndex{}, rowIndex, rowIndex);
    settingsWidgets_.push_back({name, widget});
    endInsertRows();
}
