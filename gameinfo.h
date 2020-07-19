#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <QJsonObject>
#include <QVector>

/**
 * Represents the 'game' object of the release.json
 */
class GameInfo
{
public:
    struct MirrorInfo{
        QString name;
        QString url;
    };

    GameInfo();
    GameInfo( QJsonObject );

    QString instructions_hash() const;
    QString patch_path() const;
    QString version_name() const;
    int version_number() const;
    QVector<MirrorInfo> mirrorInfo() const;

private:
    QString m_instructionsHash;
    QString m_patchPath;
    QString m_versionName;
    int m_versionNumber;
    QVector<MirrorInfo> m_mirrorInfo;
};

#endif // GAMEINFO_H
