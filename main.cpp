#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

#include "renx-config.h"

static std::shared_ptr<QSettings> m_settings;

std::shared_ptr<QSettings> renx_settings(){
    return m_settings;
}

QString renx_baseInstallPath(){
    return m_settings->value( "wine/wineprefix" ).toString()
            + "/"
            + m_settings->value( "wine/renx-install-path" ).toString();
}

static void initialize_settings(){
    m_settings = std::shared_ptr<QSettings>( new QSettings( "rm5248", "RenegadeXLauncher" ) );

    QString defaultWineLocation = "~/.wine";
    QString defaultInstallLocation = "drive_c/Program Files (x86)/Renegade X";

    if( !m_settings->contains( "wine/wineprefix" ) ){
        m_settings->setValue( "wine/wineprefix", defaultWineLocation );
    }

    if( !m_settings->contains( "wine/renx-install-path" ) ){
        m_settings->setValue( "wine/renx-install-path", defaultInstallLocation );
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // The first thing to do is to set all of our default settings
    initialize_settings();

    MainWindow w;
    w.show();

    QTimer::singleShot( 0, &w, &MainWindow::refresh );
    QTimer::singleShot( 0, &w, &MainWindow::checkForUpdates );

    return a.exec();
}
