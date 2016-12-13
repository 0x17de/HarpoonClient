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

    std::list<std::shared_ptr<Channel>> getChannels();
    int getChannelIndex(Channel* channel);
    int getChannelIndex(const QString& channelName);
    Channel* getChannel(int row);
    Channel* getChannel(const QString& channelName);
    void reconnectEvents();

signals:
    void expand(const QModelIndex& index);
    void channelConnected(Channel* channel);

    void beginInsertChannel(Server* server, int row);
    void endInsertChannel();
    void beginRemoveChannel(Server* server, int row);
    void endRemoveChannel();
    void channelDataChanged(Server* server, int row);

public Q_SLOTS:
    void channelDataChanged(Channel* channel);
    void resetChannels(std::list<std::shared_ptr<Channel>>& servers);
    void newChannel(std::shared_ptr<Channel> server);
    void deleteChannel(const QString& serverId);

private:
    std::list<std::shared_ptr<Channel>> channels_;
};

#endif
