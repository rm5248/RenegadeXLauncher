#ifndef RELEASEINFORMATION_H
#define RELEASEINFORMATION_H

#include <QJsonObject>
#include <QJsonDocument>

#include "json-objects/launcherinfo.h"
#include "json-objects/gameinfo.h"

class ReleaseInformation
{
public:
    ReleaseInformation();
    ReleaseInformation( QJsonDocument object );

    LauncherInfo launcherInfo() const;
    GameInfo gameInfo() const;

private:
    LauncherInfo m_launcherInfo;
    GameInfo m_gameInfo;
};

#endif // RELEASEINFORMATION_H
