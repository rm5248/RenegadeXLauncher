#include <QJsonArray>

#include "gameinfo.h"

GameInfo::GameInfo()
{

}

GameInfo::GameInfo( QJsonObject obj ){
    bool ok;

    m_instructionsHash = obj[ QString("instructions_hash") ].toString();
    m_patchPath = obj[ QString("patch_path") ].toString();
    m_versionName = obj[ QString("version_name") ].toString();
    m_versionNumber = obj[ QString("version_number") ].toInt();
    if( !ok ){
        // TODO handle error
    }

    QJsonArray mirrorArray = obj[ QString( "mirrors" ) ].toArray();
    for( QJsonValue val : mirrorArray ){
        MirrorInfo mi;

        mi.name = val.toObject()[ "name" ].toString();
        mi.url = val.toObject()[ "url" ].toString();

        m_mirrorInfo.push_back( mi );
    }
}

QString GameInfo::instructions_hash() const{
    return m_instructionsHash;
}

QString GameInfo::patch_path() const{
    return m_patchPath;
}

QString GameInfo::version_name() const{
    return m_versionName;
}

int GameInfo::version_number() const{
    return m_versionNumber;
}

QVector<GameInfo::MirrorInfo> GameInfo::mirrorInfo() const{
    return m_mirrorInfo;
}
