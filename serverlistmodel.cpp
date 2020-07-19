#include <QDebug>

#include "serverlistmodel.h"

ServerListModel::ServerListModel(QObject* parent) :
    QAbstractTableModel(parent)
{

}

void ServerListModel::setServerInformation(QVector<ServerInformation> information){
    m_serverInformation = information;

    qDebug() << "server information is " << m_serverInformation.length();

    emit layoutChanged();
}

int ServerListModel::columnCount(const QModelIndex &parent) const{
    return 3;
}

int ServerListModel::rowCount(const QModelIndex &parent) const{
    return m_serverInformation.length();
}

QVariant ServerListModel::data(const QModelIndex &index, int role) const{
    if( role != Qt::DisplayRole ){
        return QVariant();
    }

    ServerInformation i = m_serverInformation[ index.row() ];

    switch( index.column() ){
    case 0:
        return QVariant( i.getServerName() );
    case 1:
        return QVariant( i.getMap() );
    case 2:
        return QVariant( QString( "%1/%2" ).arg( i.getPlayers() ).arg( i.getMaxPlayers() ) );
    }

    return QVariant();
}

Qt::ItemFlags ServerListModel::flags(const QModelIndex &index) const {
    return Qt::ItemFlag::ItemIsEnabled;
}

QVariant ServerListModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if( role != Qt::DisplayRole ){
        return QVariant();
    }

    if( orientation != Qt::Orientation::Horizontal ){
        return QVariant();
    }

    switch( section ){
    case 0:
        return QVariant( "Server Name" );
    case 1:
        return QVariant( "Map Name" );
    case 2:
        return QVariant( "Players" );
    }

    return QVariant();
}
