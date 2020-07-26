#ifndef RENXINSTALLER_H
#define RENXINSTALLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QFile>

#include "json-objects/gameinfo.h"
#include "json-objects/instructionentry.h"

class RenxInstaller : public QObject
{
    Q_OBJECT
public:
    explicit RenxInstaller(QObject *parent = nullptr);

    void setMirrors( QVector<GameInfo::MirrorInfo> mirrors );
    void setPreferredMirror( GameInfo::MirrorInfo mirror );
    void setPatchPath( QString path );
    void setInstructionsHash( QString instructionsHash );
    void setNetworkAccessManager( QNetworkAccessManager* network );

signals:
    void percentDownloaded( double );
    void done();

public slots:
    void start();

private slots:
    void downloadReadyRead();
    void downloadFinished();

private:
    void determineDifferences();
    void downloadNextFile();

private:
    QVector<GameInfo::MirrorInfo> m_mirrors;
    GameInfo::MirrorInfo m_preferred;
    QString m_patchPath;
    QString m_instructionsHash;
    QNetworkAccessManager* m_networkAccess;
    QVector<InstructionEntry> m_instructions;
    QQueue<QString> m_filesToDownload;
    int m_bytesToDownload;
    int m_currentBytesDownloaded;
    QNetworkReply* m_currentDownload;
    QFile* m_currentDownloadTempFile;
};

#endif // RENXINSTALLER_H
