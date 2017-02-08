#ifndef HOSTTREEMODELIRC_H
#define HOSTTREEMODELIRC_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class IrcHost;
class IrcHostTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit IrcHostTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

signals:
    void expand(const QModelIndex& index);

public Q_SLOTS:
    void resetHosts(std::list<std::shared_ptr<IrcHost>>& servers);
    void newHost(std::shared_ptr<IrcHost> server);
    void deleteHost(const QString& host, int port);

private:
    std::list<std::shared_ptr<IrcHost>> hosts_;
};

#endif
