#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QDir>

#include "renxinstaller.h"
#include "renx-config.h"

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
    QRandomGenerator randGen;
    int randomMirrorNumber = randGen.bounded( 0, m_mirrors.length() );
    GameInfo::MirrorInfo mirrorToUse = m_mirrors[ randomMirrorNumber ];
    m_preferred = mirrorToUse;

    req.setUrl( QUrl( mirrorToUse.url + "/" + m_patchPath + "/instructions.json" ) );

    QNetworkReply* reply = m_networkAccess->get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            qDebug() << "Error: " << reply->errorString();
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            qDebug() << "Bad JSON document";
            return;
        }

        QJsonArray installationArray = jsonDoc.array();
        for( QJsonValue obj : installationArray ){
            m_instructions.push_back( InstructionEntry( obj.toObject() ) );
        }

        // Now we need to figure out if there are files that have changed.
        determineDifferences();

        emit percentDownloaded( 0 );

        // Start the download for the next file
        downloadNextFile();
    });
}

void RenxInstaller::determineDifferences(){
    QDir baseDir = renx_baseInstallPath();
    m_bytesToDownload = 0;
    m_currentBytesDownloaded = 0;

    for( InstructionEntry entry : m_instructions ){
        QFile file( baseDir.path() + "/" + entry.path() );

        if( !file.exists() ){
            m_bytesToDownload += entry.fullReplaceSize();
            m_filesToDownload.append( entry.compressedHash() );
            continue;
        }

        QFileInfo finfo( file );
        if( finfo.size() != entry.fullReplaceSize() ){
            m_bytesToDownload += entry.fullReplaceSize();
            m_filesToDownload.append( entry.compressedHash() );
            continue;
        }
    }
}

void RenxInstaller::downloadNextFile(){
    QNetworkRequest req;

    if( m_filesToDownload.empty() ){
        emit percentDownloaded( 100.0 );
        return;
    }

    QString nextDownloadFileName = m_filesToDownload.dequeue();

    qDebug() << "Downloading next file " << nextDownloadFileName;

    // Create the temporary file to download to.
    QDir tempDir = renx_baseInstallPath() + "/download";
    if( !tempDir.exists() ){
        tempDir.mkdir( renx_baseInstallPath() + "/download" );
    }

    m_currentDownloadTempFile = new QFile( tempDir.filePath( nextDownloadFileName ) );
    m_currentDownloadTempFile->open( QIODevice::WriteOnly );

    req.setUrl( QUrl( m_preferred.url + "/" + m_patchPath + "/full/" + nextDownloadFileName ) );

    m_currentDownload = m_networkAccess->get( req );
    connect( m_currentDownload, &QNetworkReply::readyRead,
             this, &RenxInstaller::downloadReadyRead );
    connect( m_currentDownload, &QNetworkReply::finished,
             this, &RenxInstaller::downloadFinished );
}

void RenxInstaller::downloadReadyRead(){
    QByteArray data = m_currentDownload->readAll();
    m_currentBytesDownloaded += data.length();

    m_currentDownloadTempFile->write( data );

    emit percentDownloaded( (m_currentBytesDownloaded / static_cast<double>( m_bytesToDownload ) ) * 100.0 );
}

void RenxInstaller::downloadFinished(){
    if( m_currentDownloadTempFile ){
        m_currentDownloadTempFile->close();
        delete m_currentDownloadTempFile;
    }

    downloadNextFile();
}
