#ifndef BACKLOGMODEL_H
#define BACKLOGMODEL_H

#include <QAbstractItemModel>
#include <array>


class BacklogModel : public QAbstractItemModel {
public:
    explicit BacklogModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void addMessage(const QString& time,
                    const QString& nick,
                    const QString& message);
private:
    typedef std::array<QString, 3> ChatLine;
    std::list<ChatLine> messages_;
};

#endif
