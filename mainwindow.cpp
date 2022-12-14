#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QVariant>
#include <QMessageBox>
#include <QDir>

#include <log4cxx/logger.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json-objects/releaseinformation.h"
#include "settingsdialog.h"
#include "json-objects/instructionentry.h"
#include "renx-config.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.MainWindow" );
static log4cxx::LoggerPtr logger_wine = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.MainWindow.wine" );

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->serverTable->setModel( &m_model );
    ui->serverTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    connect( &m_installer, &RenxInstaller::totalPercentDownloaded,
             &m_downloadProgress, &DownloadDialog::totalDownlaodPercentageUpdate );
    connect( &m_installer, &RenxInstaller::filePercentDownloaded,
             &m_downloadProgress, &DownloadDialog::downloadPercentageUpdated );
    connect( &m_installer, &RenxInstaller::fileDownloadProgress,
             &m_downloadProgress, &DownloadDialog::numFilesDownloadProgress );

    connect( &m_winetricks, &QProcess::readyReadStandardError,
             [this](){
        QByteArray ba = m_winetricks.readAllStandardError();
        LOG4CXX_DEBUG(logger, "Winetricks stderr: " << ba.toStdString() );
    });
    connect( &m_winetricks, &QProcess::readyReadStandardOutput,
             [this](){
        QByteArray ba = m_winetricks.readAllStandardOutput();
        LOG4CXX_DEBUG(logger, "Winetricks stdout: " << ba.toStdString() );
    });
    connect( &m_winetricks, &QProcess::finished,
             [this](int exitCode, QProcess::ExitStatus stat){
        LOG4CXX_DEBUG( logger, "Winetricks exited code " << exitCode );
    });

    connect( &m_game, &QProcess::readyReadStandardOutput,
             [this](){
        QByteArray ba = m_game.readAllStandardOutput();
        LOG4CXX_DEBUG(logger_wine, ba.toStdString() );
    });
    connect( &m_game, &QProcess::readyReadStandardError,
             [this](){
        QByteArray ba = m_game.readAllStandardError();
        LOG4CXX_DEBUG(logger_wine, ba.toStdString() );
    });
    connect( &m_game, &QProcess::finished,
             [this](int exitCode, QProcess::ExitStatus stat){
        LOG4CXX_DEBUG(logger_wine, "Wine exited, code " << exitCode );
        ui->statusBar->showMessage( "Game exited", 8000 );
    });
    connect( &m_game, &QProcess::started,
             [this](){
        ui->statusBar->showMessage( "Game running" );
    });

    connect( &m_installer, &RenxInstaller::allFilesDownloaded,
             [this](){
        int versionNumber = m_releaseInfo.gameInfo().version_number();
        std::shared_ptr<QSettings> settings = renx_settings();
        settings->setValue( "installed-version", versionNumber );
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh(){
    QNetworkRequest req;
    req.setUrl( QUrl( "https://serverlist-rx.totemarts.services/servers.jsp?id=launcher HTTP/1.1" ) );
//    req.setRawHeader( "User-Agent:", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) ta-games-launcher/1.0.12 Chrome/106.0.5249.181 Electron/21.3.0 Safari/537.36" );

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            LOG4CXX_ERROR( logger, "Unable to download servers: " << reply->errorString().toStdString() );
            return;
        }

        QVector<ServerInformation> info;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        LOG4CXX_DEBUG( logger, "Server JSON data: " << jsonDoc.toJson().toStdString() );

        if( jsonDoc.isNull() ){
            LOG4CXX_ERROR( logger, "Server list not valid JSON" );
            return;
        }

        if( !jsonDoc.isArray() ){
            LOG4CXX_ERROR( logger, "Server list not an array" );
            return;
        }

        for( QJsonValue val : jsonDoc.array() ){
            ServerInformation i( val.toObject() );
            if( i.isValid() ){
                info.append( i );
            }
        }

        m_model.setServerInformation( info );

    });
}

void MainWindow::on_refreshButton_clicked()
{
    refresh();
}

void MainWindow::checkForUpdates(){
    QNetworkRequest req;
    req.setUrl( QUrl( "https://static.ren-x.com/launcher_data/version/release.json" ) );

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            LOG4CXX_ERROR( logger, "Unable to download release info: " << reply->errorString().toStdString() );
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            LOG4CXX_ERROR( logger, "Release info invalid JSON" );
            return;
        }

        ReleaseInformation ri( jsonDoc );

        m_releaseInfo = ri;

        std::shared_ptr<QSettings> settings = renx_settings();
        int installedVersion = settings->value( "installed-version", QVariant( 0 ) ).toInt();

        LOG4CXX_DEBUG( logger, "Latest version is: "
                       << ri.gameInfo().version_number()
                       << " Installed version is: "
                       << installedVersion );


        m_installer.setMirrors( ri.gameInfo().mirrorInfo() );
        m_installer.setPatchPath( ri.gameInfo().patch_path() );
        m_installer.setInstructionsHash( ri.gameInfo().instructions_hash() );
        m_installer.setNetworkAccessManager( &m_network );

        if( ri.gameInfo().version_number() > installedVersion ){
            QString installQuestion = QString( "Update available!  Installed: %1 Available: %2" )
                    .arg( installedVersion )
                    .arg( ri.gameInfo().version_number() );
            QMessageBox::StandardButton response =
                    QMessageBox::question( this, "Update available!", installQuestion );

            if( response == QMessageBox::Yes ){
                m_installer.start();
                m_downloadProgress.open();
            }
        }

    });
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog d;
    d.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionValidate_Install_triggered()
{
    QNetworkRequest req;
    QVector<GameInfo::MirrorInfo> mirrors = m_releaseInfo.gameInfo().mirrorInfo();
    int randomMirrorNumber = QRandomGenerator::global()->bounded( 0, mirrors.length() );
    GameInfo::MirrorInfo mirrorToUse = mirrors[ randomMirrorNumber ];

    req.setUrl( QUrl( mirrorToUse.url + "/" + m_releaseInfo.gameInfo().patch_path() + "/instructions.json" ) );

    m_validationDialog.open();

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            LOG4CXX_ERROR( logger, "Unable to download instructions: " << reply->errorString().toStdString() );
            return;
        }

        QVector<ServerInformation> info;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            LOG4CXX_ERROR( logger, "Instructions info invalid JSON" );
            return;
        }

        QJsonArray installationArray = jsonDoc.array();
        QVector<InstructionEntry> instructions;
        for( QJsonValue obj : installationArray ){
            instructions.push_back( InstructionEntry( obj.toObject() ) );
        }

        m_validationDialog.setValidationData( instructions );
    });
}

void MainWindow::on_actionInstall_triggered()
{
    m_installer.start();
    m_downloadProgress.open();
}

void MainWindow::on_actionLaunch_Game_triggered()
{
    launchGame(QStringList());
}

void MainWindow::on_actionPerform_Winetricks_triggered()
{
    //WINEPREFIX=~/.wine-renx/ winetricks d9vk040 corefonts vcrun2008 vcrun2010 xact win7 dotnet452
    //winetricks corefonts vcrun2008 vcrun2010 xact xact_x64 d3dx9 d3dx9_43 msxml3 dotnet452 win7
    // 2022-10-28: also did winetricks with d9vk. possibly dxvk works fine now too

    QStringList winetricksArgs;
    winetricksArgs
            << "corefonts"
            << "vcrun2008"
            << "vcrun2010"
            << "xact"
            << "xact_x64"
            << "d3dx9"
            << "d3dx9_43"
            << "msxml3"
            << "dotnet452"
            << "win7";

    std::shared_ptr<QSettings> settings = renx_settings();

    QProcessEnvironment currentEnv = QProcessEnvironment::systemEnvironment();
    QString installDir = settings->value( "wine/wineprefix" ).toString();
    installDir.replace( "~", QDir::homePath() );
    installDir.append( "/" );
    currentEnv.insert( "WINEPREFIX", installDir );

    m_winetricks.setProcessEnvironment( currentEnv );
    m_winetricks.setProgram( "winetricks" );
    m_winetricks.setArguments( winetricksArgs );
    m_winetricks.start();
}

void MainWindow::on_serverTable_doubleClicked(const QModelIndex &index)
{
    LOG4CXX_DEBUG(logger, "Launching game");
    ServerInformation si = m_model.serverInformationAtRow(index.row());

    QStringList extraArgs;
    QString extraArg = QString("%1:%2").arg(si.getIP()).arg(si.getPort());
    std::shared_ptr<QSettings> settings = renx_settings();
    QVariant username = settings->value( "username" );
    if( username.isValid() ){
        extraArg.append( "?Name=" + username.toString() );
    }
    extraArgs.append( extraArg );

    launchGame( extraArgs );
}

void MainWindow::launchGame(QStringList extraArgs){
    std::shared_ptr<QSettings> settings = renx_settings();

    QProcessEnvironment currentEnv = QProcessEnvironment::systemEnvironment();
    QString installDir = settings->value( "wine/wineprefix" ).toString();
    installDir.replace( "~", QDir::homePath() );
    installDir.append( "/" );
    currentEnv.insert( "WINEPREFIX", installDir );

    QStringList dxvkSettings;
    if( settings->value( "dxvk/devinfo", "false" ).toBool() ){
        dxvkSettings.push_back( "devinfo" );
    }
    if( settings->value( "dxvk/fps", "false" ).toBool() ){
        dxvkSettings.push_back( "fps" );
    }
    if( settings->value( "dxvk/version", "false" ).toBool() ){
        dxvkSettings.push_back( "version" );
    }
    if( !dxvkSettings.isEmpty() ){
        QString fullSettings;
        for( QString str : dxvkSettings ){
            fullSettings.append( str );
            fullSettings.append( "," );
        }
        fullSettings.remove( fullSettings.size() - 1, 1 );
        currentEnv.insert( "DXVK_HUD", fullSettings );
    }

    QStringList processArgs;

    QString udkExeStr( "/Binaries/%1/UDK.exe" );
    if( settings->value( "use-64bit", "false" ).toBool() ){
        udkExeStr = udkExeStr.arg( "Win32" );
        currentEnv.insert( "LD_PRELOAD", QDir::homePath() + "/.local/share/Steam/ubuntu12_32/gameoverlayrenderer.so" );
    }else{
        udkExeStr = udkExeStr.arg( "Win64" );
        currentEnv.insert( "LD_PRELOAD", QDir::homePath() + "/.local/share/Steam/ubuntu12_32/gameoverlayrenderer.so" );
    }
    QFile udkExe( renx_baseInstallPath() + udkExeStr );
    QFileInfo fi(udkExe);

    LOG4CXX_DEBUG(logger, "Absolute path: " << fi.absoluteFilePath().toStdString() );

    processArgs.append( fi.absoluteFilePath() );
    processArgs.append( extraArgs );
    processArgs.append( "-nomovies" );

    m_game.setProcessEnvironment( currentEnv );
    m_game.setProgram( "wine" );
    m_game.setArguments( processArgs );
    m_game.start();
}
