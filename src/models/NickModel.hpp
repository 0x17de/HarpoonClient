#ifndef NICKMODEL_H
#define NICKMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class NickModel : public QAbstractItemModel {
    Q_OBJECT

    std::list<QString> nicks_;

public:
    explicit NickModel(QObject* parent = 0);

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
    void resetNicks(std::list<QString>& nicks);
    void newNick(const QString& nick);
    void deleteNick(const QString& host, int port);
};

#endif
