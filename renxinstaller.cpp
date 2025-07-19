#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QDir>

#include <log4cxx/logger.h>

#include "renxinstaller.h"
#include "renx-config.h"
#include "filepatcher.h"
#include "filevalidator.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.RenxInstaller" );

RenxInstaller::RenxInstaller(QObject *parent) : QObject(parent),
    m_currentDownloadTempFile( nullptr )
{

}

void RenxInstaller::setMirrors( QVector<GameInfo::MirrorInfo> mirrors ){
    m_mirrors = mirrors;
}

void RenxInstaller::setPreferredMirror( GameInfo::MirrorInfo mirror ){
    m_preferred = mirror;
}

void RenxInstaller::setPatchPath( QString path ){
    m_patchPath = path;
}

void RenxInstaller::setInstructionsHash( QString instructionsHash ){
    m_instructionsHash = instructionsHash;
}

void RenxInstaller::setNetworkAccessManager( QNetworkAccessManager* network ){
    m_networkAccess = network;
}

void RenxInstaller::start(){
    // First we need to download the instructions
    QNetworkRequest req;
    int randomMirrorNumber = QRandomGenerator::global()->bounded( 0, m_mirrors.length() );
    GameInfo::MirrorInfo mirrorToUse = m_mirrors[ randomMirrorNumber ];
    m_preferred = mirrorToUse;

    req.setUrl( QUrl( mirrorToUse.url + "/" + m_patchPath + "/instructions.json" ) );

    LOG4CXX_DEBUG( logger, "Downloading instructions from " << req.url().toString().toStdString() );

    QNetworkReply* reply = m_networkAccess->get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            LOG4CXX_ERROR( logger, "Unable to download instructions: " << reply->errorString().toStdString() );
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            LOG4CXX_ERROR( logger, "Invalid JSON file for instructions!" );
            return;
        }

        QJsonArray installationArray = jsonDoc.array();
        for( QJsonValue obj : installationArray ){
            m_instructions.push_back( InstructionEntry( obj.toObject() ) );
            if( m_instructions.last().oldHash().isEmpty() ){
                LOG4CXX_ERROR( logger, "bbb" );
            }
        }

        // Now we need to figure out if there are files that have changed.
        determineDifferences();

        emit totalProgress(0, m_numFilesDownloadTracker.current, m_numFilesDownloadTracker.max);

        // Start the download for the next file
        downloadNextFile();
    });
}

void RenxInstaller::determineDifferences(){
    QDir baseDir = renx_baseInstallPath();
    m_fileChecksumTracker.clear();
    m_currentFileDownloadTracker.clear();
    m_totalBytesDownloadTracker.clear();
    m_numFilesDownloadTracker.clear();

    int currentValidation = 1;

    m_fileChecksumTracker.max = m_instructions.size();

    for( InstructionEntry entry : m_instructions ){
        currentValidation++;

        QFile file( baseDir.path() + "/" + entry.path() );

        if( entry.newHash().isEmpty() ){
            LOG4CXX_DEBUG( logger, "File " << entry.path().toStdString()
                           << " has empty new hash, deleting" );
            file.remove();
            continue;
        }

        if( !file.exists() ){
            LOG4CXX_DEBUG( logger, "File " << entry.path().toStdString()
                           << " does not exist, adding to download queue" );
            m_totalBytesDownloadTracker.max += entry.fullReplaceSize();
            m_filesToDownload.append( entry );
            m_fileChecksumTracker.current++;
            m_numFilesDownloadTracker.max++;
            continue;
        }

        // Checksum the file, see if it is different.
        QFileInfo fi(file);
        FileValidator* validator = new FileValidator( fi.absoluteFilePath(),
                                                      entry.newHash() );
        validator->moveToThread( &m_checksumThread );

        connect( validator, &FileValidator::checksumCompleted,
                 [this,entry]( bool success, QString ){
            QDir baseDir = renx_baseInstallPath();
            QFile file( baseDir.path() + "/" + entry.path() );
            QFileInfo finfo( file );

            if( !success ){
                LOG4CXX_DEBUG( logger, "File " << finfo.absoluteFilePath().toStdString()
                               << " checksum did not match, downloading" );
                m_filesToDownload.push_back(entry);
                m_totalBytesDownloadTracker.max += entry.fullReplaceSize();
                m_numFilesDownloadTracker.max++;
            }else{
                LOG4CXX_DEBUG( logger, "File " << finfo.absoluteFilePath().toStdString()
                               << " checksum matched, not downloading" );
            }

            // try to download the next file if we are not downloading anything
            downloadNextFile();
                 });
        connect( validator, &FileValidator::checksumCompleted,
                 [validator](){
            validator->deleteLater();
        }
                 );
        connect( &m_checksumThread, &QThread::started,
                 validator, &FileValidator::startChecksum );
        connect( validator, &FileValidator::checksumStarting,
                 this, &RenxInstaller::fileChecksumStarting );
    }

    emit validationProgress( "", m_numFilesDownloadTracker.getPercentage(), m_numFilesDownloadTracker.current, m_numFilesDownloadTracker.max );
    m_checksumThread.start();
}

void RenxInstaller::downloadNextFile(){
    QNetworkRequest req;

    if( m_filesToDownload.empty() ){
        emit totalProgress(100, m_numFilesDownloadTracker.current, m_numFilesDownloadTracker.max);
        emit installationCompleted();
        return;
    }

    if( m_currentDownloadTempFile ){
        return;
    }

    m_currentInstruction = m_filesToDownload.dequeue();
    QString nextDownloadFileName = m_currentInstruction.newHash();
    m_currentFileDownloadTracker.clear();
    m_currentFileDownloadTracker.max = m_currentInstruction.fullReplaceSize();
    m_numFilesDownloadTracker.current++;

    LOG4CXX_DEBUG( logger, "Downloading the next file: " << nextDownloadFileName.toStdString() );

    emit fileDownloadProgress(m_currentInstruction.path(), m_currentFileDownloadTracker.getPercentage(), 0, m_currentFileDownloadTracker.max);

    // Create the temporary file to download to.
    QDir tempDir = renx_baseInstallPath() + "/download";
    if( !tempDir.exists() ){
        if( !tempDir.mkpath( "." ) ){
            LOG4CXX_ERROR( logger, "can't mkpath!  dir: " << tempDir.absolutePath().toStdString() );
            return;
        }
    }

    m_currentDownloadTempFile = new QFile( tempDir.filePath( nextDownloadFileName ) );
    m_currentDownloadTempFile->open( QIODevice::WriteOnly );

    req.setUrl( QUrl( m_preferred.url + "/" + m_patchPath + "/full/" + nextDownloadFileName ) );

    LOG4CXX_DEBUG( logger, "Downloading file full path: " << req.url().toString().toStdString() );

    m_currentDownload = m_networkAccess->get( req );
    connect( m_currentDownload, &QNetworkReply::readyRead,
             this, &RenxInstaller::downloadReadyRead );
    connect( m_currentDownload, &QNetworkReply::finished,
             this, &RenxInstaller::downloadFinished );
//    connect( m_currentDownload, &QNetworkReply::downloadProgress,
//             [this](qint64 bytesRx, qint64 totalBytes){
//        emit fileDownloadProgress(m_currentInstruction.path(), bytesRx / totalBytes * 100.0, bytesRx, totalBytes );
//    });
}

void RenxInstaller::downloadReadyRead(){
    QByteArray data = m_currentDownload->readAll();
    m_currentFileDownloadTracker.current += data.length();
    m_totalBytesDownloadTracker.current += data.length();

    m_currentDownloadTempFile->write( data );

    double percent = m_currentFileDownloadTracker.getPercentage();
    LOG4CXX_TRACE( logger, "Download: " <<
                   m_currentFileDownloadTracker.current
                   << "/"
                   << m_currentFileDownloadTracker.max
                   << " = "
                   << percent
                   << "%" );

    emit fileDownloadProgress(m_currentInstruction.path(), m_currentFileDownloadTracker.getPercentage(), m_currentFileDownloadTracker.current, m_currentFileDownloadTracker.max);
}

void RenxInstaller::downloadFinished(){
    m_currentDownload->deleteLater();
    m_currentDownloadTempFile->close();

    emit fileDownloadProgress(m_currentInstruction.path(), m_currentFileDownloadTracker.getPercentage(), m_numFilesDownloadTracker.current, m_numFilesDownloadTracker.max);

    if( m_currentDownload->error() != QNetworkReply::NoError ){
        LOG4CXX_ERROR( logger, "Unable to download file!" );
    }else{
        LOG4CXX_DEBUG( logger, "File downloaded successfully" );

        if( m_currentInstruction.path().isEmpty() ){
            LOG4CXX_ERROR( logger, "current instruction path empty!" );
        }

        QFile endingFile( renx_baseInstallPath() + "/" + m_currentInstruction.path() );
        QFileInfo endingInfo( endingFile );

        QDir theDir = endingInfo.dir();
        if( !theDir.exists() ){
            LOG4CXX_DEBUG( logger, "Directory "
                           << theDir.absolutePath().toStdString()
                           << " does not exist, creating" );
            if( !theDir.mkpath( "." ) ){
                LOG4CXX_ERROR( logger, "Can't create directory!" );
            }
        }

        FilePatcher* fpatch = new FilePatcher();
        QFileInfo fi(*m_currentDownloadTempFile);
        fpatch->setInputFile( fi.absoluteFilePath() );
        fpatch->setOutputFile( endingInfo.absoluteFilePath() );
        connect( fpatch, &FilePatcher::filePatched,
                 [fpatch](){
           LOG4CXX_DEBUG( logger, "File patched! "
                          << fpatch->inputFile().toStdString()
                          << " -> "
                          << fpatch->outputFile().toStdString() );
           fpatch->deleteLater();
        });
        fpatch->doPatch();

        emit totalProgress(m_numFilesDownloadTracker.getPercentage(), m_numFilesDownloadTracker.current, m_numFilesDownloadTracker.max);
    }

    delete m_currentDownloadTempFile;
    m_currentDownloadTempFile = nullptr;

    downloadNextFile();
}

void RenxInstaller::fileChecksumStarting( QString fileName ){
    m_fileChecksumTracker.current++;
    emit validationProgress( fileName, m_fileChecksumTracker.getPercentage(), m_fileChecksumTracker.current, m_fileChecksumTracker.max );
}
