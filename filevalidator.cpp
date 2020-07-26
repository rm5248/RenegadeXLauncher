#include <QCryptographicHash>
#include <QFileInfo>

#include <log4cxx/logger.h>

#include "filevalidator.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.FileValidator" );

FileValidator::FileValidator(QString fileToChecksum,
                             QString expectedChecksum,
                             QObject *parent) : QObject(parent),
    m_file( fileToChecksum ),
    m_expected( expectedChecksum.toUpper() )
{

}

QString FileValidator::calculatedChecksum() const{
    return m_checksum;
}

QString FileValidator::expectedChecksum() const{
    return m_expected;
}

void FileValidator::startChecksum(){
    if( !m_file.exists() ){
        emit checksumCompleted( false, QString() );
        return;
    }

    if( !m_file.open( QIODevice::ReadOnly ) ){
        QFileInfo inf( m_file );
        LOG4CXX_ERROR( logger, "Unable to open file "
                       << inf.absoluteFilePath().toStdString()
                       << ": "
                       << m_file.errorString().toStdString() );
        emit checksumCompleted( false, QString() );
        return;
    }

    QCryptographicHash hash( QCryptographicHash::Sha256 );
    hash.addData( &m_file );

    m_file.close();

    m_checksum = hash.result().toHex().toUpper();
    bool result = m_checksum == m_expected;

    emit checksumCompleted( result, m_checksum );
}
