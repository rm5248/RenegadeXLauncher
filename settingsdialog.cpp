#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "renx-config.h"

#include <QSettings>
#include <QStandardPaths>
#include <QProcess>
#include <QtDebug>
#include <QMessageBox>
#include <QDir>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.SettingsDialog" );

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_winecfg( nullptr )
{
    ui->setupUi(this);

    std::shared_ptr<QSettings> settings = renx_settings();

    QString wineLocation = settings->value( "wine/wineprefix" ).toString();
    ui->wineprefix->setText( wineLocation );
    ui->installLocation->setText( settings->value( "wine/renx-install-path" ).toString() );
    ui->username->setText( settings->value( "username" ).toString() );

    connect( this, &QDialog::accepted,
             this, &SettingsDialog::settingsAccepted );
//    connect( &m_winecfg, &QProcess::readyReadStandardError,
//             this, &SettingsDialog::wineOutput );
//    connect( &m_winecfg, &QProcess::readyReadStandardOutput,
//             this, &SettingsDialog::wineOutput );
    connect( &m_winecfg, static_cast<void (QProcess::*)(int,QProcess::ExitStatus)>( &QProcess::finished ),
             this, &SettingsDialog::wineConfigFinished );
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::settingsAccepted(){
    std::shared_ptr<QSettings> settings = renx_settings();
    settings->setValue( "wine/wineprefix", ui->wineprefix->text() );
    settings->setValue( "wine/renx-install-path", ui->installLocation->text() );
    settings->setValue( "use-64bit", ui->use64Bit->isChecked() );
    settings->setValue( "username", ui->username->text() );
}

void SettingsDialog::on_launchWinecfg_clicked()
{
    if( m_winecfg.state() != QProcess::NotRunning ){
        return;
    }

    std::shared_ptr<QSettings> settings = renx_settings();

    QProcessEnvironment currentEnv = QProcessEnvironment::systemEnvironment();
    QString installDir = settings->value( "wine/wineprefix" ).toString();
    installDir.replace( "~", QDir::homePath() );
    installDir.append( "/" );
    currentEnv.insert( "WINEPREFIX", installDir );

    m_winecfg.setProcessEnvironment( currentEnv );

    QStringList args;
    args.push_back( "winecfg" );

    m_winecfg.start( "wine", args );
}

void SettingsDialog::wineOutput(){
    QByteArray stdErr = m_winecfg.readAllStandardError();

    qDebug() << stdErr;

    QByteArray stdOut = m_winecfg.readAllStandardOutput();

    qDebug() << stdOut;
}

void SettingsDialog::wineConfigFinished(int exitCode, QProcess::ExitStatus status){
    if( exitCode != 0 && status == QProcess::ExitStatus::NormalExit ){
        QMessageBox::critical( this, "Error from Wine", m_winecfg.readAllStandardError() );
    }
}

void SettingsDialog::on_launchWinetricks_clicked()
{
    if( m_winetricks.state() != QProcess::NotRunning ){
        return;
    }

    std::shared_ptr<QSettings> settings = renx_settings();

    QProcessEnvironment currentEnv = QProcessEnvironment::systemEnvironment();
    QString installDir = settings->value( "wine/wineprefix" ).toString();
    installDir.replace( "~", QDir::homePath() );
    installDir.append( "/" );
    currentEnv.insert( "WINEPREFIX", installDir );

    LOG4CXX_DEBUG( logger, "WINEPREFIX is " << installDir.toStdString() );

    m_winetricks.setProcessEnvironment( currentEnv );

    QStringList args;

    m_winetricks.setProcessEnvironment( currentEnv );
    m_winetricks.setProgram( "winetricks" );
    m_winetricks.setArguments( args );
    m_winetricks.start();
}
