#ifndef RENXINSTALLER_H
#define RENXINSTALLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QFile>
#include <QThread>

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
    /**
     * Emitted when a file is started to be checked.
     */
    void validationProgress( QString currentFile, double percent, int currentNum, int totalNumberFiles );
    /**
     * Emitted as a file is being downloaded.
     */
    void fileDownloadProgress( QString currentFile, double percent, int currentDownloadNum, int totalNumToDownload );
    /**
     * Emitted periodically to show the current state.
     */
    void totalProgress( int percent, int currentNumber, int maxNumber );

    /**
     * Emitted once all operations have been completed
     */
    void installationCompleted();

public slots:
    void start();

private slots:
    void downloadReadyRead();
    void downloadFinished();
    void fileChecksumStarting( QString fileName );

private:
    void determineDifferences();
    void downloadNextFile();

private:
    struct PercentageTracker{
        int current = 0;
        int max = 0;

        int getPercentage(){
            return ((double)current / (double)max) * 100.0;
        }

        void clear(){
            current = 0;
            max = 0;
        }
    };

    QVector<GameInfo::MirrorInfo> m_mirrors;
    GameInfo::MirrorInfo m_preferred;
    QString m_patchPath;
    QString m_instructionsHash;
    QNetworkAccessManager* m_networkAccess;
    QVector<InstructionEntry> m_instructions;
    QQueue<InstructionEntry> m_filesToDownload;
    QNetworkReply* m_currentDownload;
    QFile* m_currentDownloadTempFile;
    InstructionEntry m_currentInstruction;
    QThread m_checksumThread;

    // Keeps track of how many files we are validating
    PercentageTracker m_fileChecksumTracker;
    // Keeps track of the download progress for the current file
    PercentageTracker m_currentFileDownloadTracker;
    // Keeps track of the total number of bytes that we are downloading
    PercentageTracker m_totalBytesDownloadTracker;
    // Keeps track of the total number of files that we are downloading
    PercentageTracker m_numFilesDownloadTracker;

};

#endif // RENXINSTALLER_H
