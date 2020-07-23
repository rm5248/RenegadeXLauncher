#ifndef SERVERINFORMATION_H
#define SERVERINFORMATION_H

#include <QString>
#include <QJsonObject>

class ServerInformation
{
public:
    ServerInformation();
    ServerInformation( QJsonObject object );

    bool isValid();
    QString getServerName();
    QString getMap();
    int getPlayers();
    int getMaxPlayers();

private:
    QString m_serverName;
    QString m_map;
    int m_bots;
    int m_players;
    int m_maxPlayers;
    bool m_isValid;
};

#endif // SERVERINFORMATION_H
