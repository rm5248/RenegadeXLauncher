#ifndef LAUNCHERINFO_H
#define LAUNCHERINFO_H

#include <QJsonObject>

/**
 * Represents the 'launcher' object of the release.json file
 */
class LauncherInfo
{
public:
    LauncherInfo();
    LauncherInfo( QJsonObject obj );
};

#endif // LAUNCHERINFO_H
