#include "releaseinformation.h"

ReleaseInformation::ReleaseInformation()
{

}

ReleaseInformation::ReleaseInformation( QJsonDocument object ){
    m_gameInfo = GameInfo( object[ "game" ].toObject() );
    m_launcherInfo = LauncherInfo( object[ "launcher" ].toObject() );
}

LauncherInfo ReleaseInformation::launcherInfo() const {
    return m_launcherInfo;
}

GameInfo ReleaseInformation::gameInfo() const {
    return m_gameInfo;
}
