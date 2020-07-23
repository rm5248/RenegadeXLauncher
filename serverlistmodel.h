#ifndef SERVERLISTMODEL_H
#define SERVERLISTMODEL_H

#include <QAbstractTableModel>
#include <QVector>

#include "json-objects/serverinformation.h"

class ServerListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ServerListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setServerInformation( QVector<ServerInformation> information );

private:
    QVector<ServerInformation> m_serverInformation;
};

#endif // SERVERLISTMODEL_H
