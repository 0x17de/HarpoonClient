#ifndef CHANNELTREEMODEL_H
#define CHANNELTREEMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class Server;
class ChannelTreeModel : public QAbstractItemModel {
public:
    explicit ChannelTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void addServers(const std::list<std::shared_ptr<Server>>& servers);

private:
    std::list<std::shared_ptr<Server>> servers_;
};

#endif
