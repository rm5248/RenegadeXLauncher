#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>

#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/logger.h>

#include "renx-config.h"

static std::shared_ptr<QSettings> m_settings;
static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher" );

std::shared_ptr<QSettings> renx_settings(){
    return m_settings;
}

QString renx_baseInstallPath(){
    QString path = m_settings->value( "wine/wineprefix" ).toString()
            + "/"
            + m_settings->value( "wine/renx-install-path" ).toString();

    path = path.replace( "~", QDir::homePath() );
    path += "/";

    return path;
}

static void initialize_settings(){
    m_settings = std::shared_ptr<QSettings>( new QSettings() );

    QString defaultWineLocation = "~/.wine";
    QString defaultInstallLocation = "drive_c/Program Files (x86)/Renegade X";

    if( !m_settings->contains( "wine/wineprefix" ) ){
        m_settings->setValue( "wine/wineprefix", defaultWineLocation );
    }

    if( !m_settings->contains( "wine/renx-install-path" ) ){
        m_settings->setValue( "wine/renx-install-path", defaultInstallLocation );
    }
}

static void qt_msg_handler(QtMsgType type, const QMessageLogContext& context, const QString& message ){
    log4cxx::LoggerPtr qtLogger = log4cxx::Logger::getLogger( context.category );
    log4cxx::spi::LocationInfo location( context.file, context.file, context.function, context.line );

    switch( type ){
    case QtMsgType::QtDebugMsg:
        qtLogger->debug( message.toStdString(), location );
        break;
    case QtMsgType::QtWarningMsg:
         qtLogger->warn( message.toStdString(), location );
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtMsgType::QtInfoMsg:
        qtLogger->info( message.toStdString(), location );
        break;
#endif
    case QtMsgType::QtCriticalMsg:
        qtLogger->error( message.toStdString(), location );
        break;
    case QtMsgType::QtFatalMsg:
        qtLogger->fatal( message.toStdString(), location );
        std::abort();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName( "rm5248" );

    QString configLocation =
            QStandardPaths::standardLocations( QStandardPaths::AppConfigLocation ).first();

    QDir d( configLocation );
    if( !d.exists() ){
        d.mkpath( "." );
    }

    QString logconfigFile = configLocation + "/logconfig.xml";
    log4cxx::xml::DOMConfigurator::configure( logconfigFile.toStdString() );
    qInstallMessageHandler( qt_msg_handler );

    LOG4CXX_INFO( logger, "Renegade-X Launcher starting up" );

    // The first thing to do is to set all of our default settings
    initialize_settings();

    MainWindow w;
    w.show();

    QTimer::singleShot( 0, &w, &MainWindow::refresh );
    QTimer::singleShot( 0, &w, &MainWindow::checkForUpdates );

    return a.exec();
}
