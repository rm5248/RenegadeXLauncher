#include <QCryptographicHash>

#include "filevalidator.h"

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
        emit checksumCompleted( false, QString() );
        return;
    }

    QCryptographicHash hash( QCryptographicHash::Sha256 );
    hash.addData( &m_file );

    m_checksum = hash.result().toHex().toUpper();
    bool result = m_checksum == m_expected;

    emit checksumCompleted( result, m_checksum );
}
