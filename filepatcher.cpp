#include "filepatcher.h"
#include <log4cxx/logger.h>
#include <QFileInfo>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.FilePatcher" );

FilePatcher::FilePatcher(QObject *parent) : QObject(parent)
{
    m_xdeltaProcess.setProgram( "xdelta3" );

    connect( &m_xdeltaProcess, &QProcess::finished,
             this, &FilePatcher::finished );
}

void FilePatcher::setInputFile(QString absolutePath){
    m_inputFile = absolutePath;
}

void FilePatcher::setOutputFile(QString absolutePath){
    m_outputFile = absolutePath;
}

void FilePatcher::doPatch(){
    QFile outputFile( m_outputFile );
    QFileInfo outputFileinfo( outputFile );
    bool isPatch = false;

    if( outputFileinfo.exists() ){
        isPatch = true;
        LOG4CXX_WARN( logger, "Output file exists, this may fail" );
    }

    QStringList args;
    args.append( "-d" );
    args.append( m_inputFile );
    args.append( m_outputFile );

    m_xdeltaProcess.setArguments( args );
    m_xdeltaProcess.start();
}

void FilePatcher::finished(int exitCode, QProcess::ExitStatus status){
    if( exitCode != 0 ){
        LOG4CXX_ERROR( logger, "Unable to patch " << m_inputFile.toStdString() << " -> " << m_outputFile.toStdString() );
        return;
    }

    emit filePatched();
}
