#ifndef CHANNELTREEMODELIRC_H
#define CHANNELTREEMODELIRC_H

#include <QAbstractItemModel>
#include <list>
#include <memory>


class IrcServer;
class IrcChannel;
class IrcChannelTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit IrcChannelTreeModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    std::list<std::shared_ptr<IrcChannel>> getChannels();
    int getChannelIndex(IrcChannel* channel);
    int getChannelIndex(const QString& channelName);
    IrcChannel* getChannel(int row);
    IrcChannel* getChannel(const QString& channelName);
    void reconnectEvents();

signals:
    void expand(const QModelIndex& index);
    void channelConnected(IrcChannel* channel);

    void beginInsertChannel(std::shared_ptr<IrcServer> server, int row);
    void newChannel(std::shared_ptr<IrcChannel> channel);
    void endInsertChannel();
    void beginRemoveChannel(std::shared_ptr<IrcServer> server, int row);
    void endRemoveChannel();
    void channelDataChanged(std::shared_ptr<IrcServer> server, int row);

public Q_SLOTS:
    void channelDataChanged(IrcChannel* channel);
    void resetChannels(std::list<std::shared_ptr<IrcChannel>>& servers);
    void addChannel(std::shared_ptr<IrcChannel> server);
    void deleteChannel(const QString& serverId);

private:
    std::list<std::shared_ptr<IrcChannel>> channels_;
};

#endif
