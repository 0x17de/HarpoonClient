#ifndef SETTINGSTYPEMODEL_H
#define SETTINGSTYPEMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <tuple>


class SettingsTypeModel : public QAbstractItemModel {
    Q_OBJECT

    std::list<QString> typeNames_;

public:
    explicit SettingsTypeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void newType(const QString& name);
    void resetTypes(const std::list<QString>& types);
};

#endif
