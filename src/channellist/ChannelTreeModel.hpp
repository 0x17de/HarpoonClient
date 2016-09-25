#ifndef CHANNELTREEMODEL_H
#define CHANNELTREEMODEL_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class Server;
class Channel;

class ChannelTreeModel : public QAbstractItemModel {
    Q_OBJECT

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

    Server* getServer(const QString& serverId);
    Channel* getChannel(Server* server, const QString& channelName);
    Channel* getChannel(const QString& serverId, const QString& channelName);
    int getServerIndex(Server* server);
    void connectServer(Server* server);
    void connectChannel(Channel* channel);
    void reconnectEvents();

signals:
    void expand(const QModelIndex& index);
    void channelConnected(Channel* channel);

public Q_SLOTS:
    void channelDataChanged(Channel* channel);
    void resetServers(std::list<std::shared_ptr<Server>>& servers);
    void newServer(std::shared_ptr<Server> server);
    void beginAddChannel(Channel* channel);
    void endAddChannel();

private:
    std::list<std::shared_ptr<Server>> servers_;
};

#endif
