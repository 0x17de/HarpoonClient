#ifndef SERVERTREEMODEL_H
#define SERVERTREEMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class Server;
class Channel;

class ServerTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ServerTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    std::list<std::shared_ptr<Server>>& getServers();
    std::shared_ptr<Server> getServer(const QString& serverId);
    int getServerIndex(Server* server);
    void connectServer(Server* server);
    void reconnectEvents();

signals:
    void expand(const QModelIndex& index);

public Q_SLOTS:
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void newServer(std::shared_ptr<Server> server);
    void deleteServer(const QString& serverId);

private:
    std::list<std::shared_ptr<Server>> servers_;
};

#endif
