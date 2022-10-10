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
            LOG4CXX_ERROR( logger, "Invalid XML file for instructions!" );
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

        emit totalPercentDownloaded( 0 );

        // Start the download for the next file
        downloadNextFile();
    });
}

void RenxInstaller::determineDifferences(){
    QDir baseDir = renx_baseInstallPath();
    m_bytesToDownload = 0;
    m_currentBytesDownloaded = 0;
    m_numFilesDownloaded = 0;

    for( InstructionEntry entry : m_instructions ){
        QFile file( baseDir.path() + "/" + entry.path() );

        if( !file.exists() ){
            m_bytesToDownload += entry.fullReplaceSize();
            m_filesToDownload.append( entry );
            continue;
        }

        QFileInfo finfo( file );
        if( finfo.size() != entry.fullReplaceSize() ){
            m_bytesToDownload += entry.fullReplaceSize();
            m_filesToDownload.append( entry );
            continue;
        }

        LOG4CXX_DEBUG( logger, "File " <<
                       finfo.absoluteFilePath().toStdString()
                       << " is good, not downloading"
                       );
    }
}

void RenxInstaller::downloadNextFile(){
    QNetworkRequest req;

    if( m_filesToDownload.empty() ){
        emit totalPercentDownloaded( 100.0 );
        emit filePercentDownloaded( 100.0 );
        return;
    }

    emit filePercentDownloaded( 0.0 );

    m_currentInstruction = m_filesToDownload.dequeue();
    QString nextDownloadFileName = m_currentInstruction.newHash();

    LOG4CXX_DEBUG( logger, "Downloading the next file: " << nextDownloadFileName.toStdString() );

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
    connect( m_currentDownload, &QNetworkReply::downloadProgress,
             [this](qint64 bytesRx, qint64 totalBytes){
        emit filePercentDownloaded( bytesRx / totalBytes * 100.0 );
    });
}

void RenxInstaller::downloadReadyRead(){
    QByteArray data = m_currentDownload->readAll();
    m_currentBytesDownloaded += data.length();

    m_currentDownloadTempFile->write( data );

    double percent = (m_currentBytesDownloaded / static_cast<double>( m_bytesToDownload ) ) * 100.0;
    LOG4CXX_TRACE( logger, "Download: " <<
                   m_currentBytesDownloaded
                   << "/"
                   << m_bytesToDownload
                   << " = "
                   << percent
                   << "%" );
    emit totalPercentDownloaded( percent );
}

void RenxInstaller::downloadFinished(){
    m_currentDownload->deleteLater();
    m_currentDownloadTempFile->close();

    emit filePercentDownloaded(100.0);

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

        // TODO actually need to do xdelta here
        LOG4CXX_DEBUG( logger, "Renaming downloaded file "
                       << m_currentInstruction.newHash().toStdString()
                       << " to "
                       << endingInfo.absoluteFilePath().toStdString() );
        m_currentDownloadTempFile->rename( endingInfo.absoluteFilePath() );


        m_numFilesDownloaded++;
        emit fileDownloadProgress( m_numFilesDownloaded, m_instructions.size() );
    }

    delete m_currentDownloadTempFile;

    downloadNextFile();
}
