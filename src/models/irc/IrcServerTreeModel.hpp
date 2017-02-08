#ifndef IRCSERVERTREEMODEL_H
#define IRCSERVERTREEMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class IrcServer;
class IrcChannel;

class IrcServerTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit IrcServerTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    std::list<std::shared_ptr<IrcServer>>& getServers();
    std::shared_ptr<IrcServer> getServer(const QString& serverId);
    int getServerIndex(IrcServer* server);
    void connectServer(IrcServer* server);
    void reconnectEvents();

signals:
    void expand(const QModelIndex& index);
    void newChannel(std::shared_ptr<IrcChannel> channel);

public Q_SLOTS:
    void resetServers(std::list<std::shared_ptr<IrcServer>>& servers);
    void newServer(std::shared_ptr<IrcServer> server);
    void deleteServer(const QString& serverId);

private:
    std::list<std::shared_ptr<IrcServer>> servers_;
};

#endif
