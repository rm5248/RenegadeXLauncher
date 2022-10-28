#include <QVariant>

#include "serverinformation.h"

ServerInformation::ServerInformation() :
    m_isValid( false )
{}

ServerInformation::ServerInformation( QJsonObject object ) :
    m_isValid( true )
{
    bool ok;

    m_serverName = object[ QString("Name") ].toString();
    m_map = object[ QString("Current Map") ].toString();
    m_bots = object[ QString("Bots") ].toVariant().toInt( &ok );
    if( !ok ) m_isValid = false;
    m_players = object[ QString("Players") ].toVariant().toInt( &ok );
    if( !ok ) m_isValid = false;
    m_maxPlayers = object[ QString("Variables") ].toObject()[ "Player Limit" ].toVariant().toInt( &ok );
    if( !ok ) m_isValid = false;
    m_ip = object[ QString("IP") ].toString();
    m_port = object[ QString("Port") ].toVariant().toInt( &ok );
    if( !ok ) m_isValid = false;
}

bool ServerInformation::isValid(){
    return m_isValid;
}

QString ServerInformation::getServerName(){
    return m_serverName;
}

QString ServerInformation::getMap(){
    return m_map;
}

int ServerInformation::getPlayers(){
    return m_players;
}

int ServerInformation::getMaxPlayers(){
    return m_maxPlayers;
}

QString ServerInformation::getIP(){
    return m_ip;
}

int ServerInformation::getPort(){
    return m_port;
}
