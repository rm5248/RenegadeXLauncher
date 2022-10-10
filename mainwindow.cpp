#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QVariant>
#include <QMessageBox>

#include <log4cxx/logger.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json-objects/releaseinformation.h"
#include "settingsdialog.h"
#include "json-objects/instructionentry.h"
#include "renx-config.h"

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.MainWindow" );

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->serverTable->setModel( &m_model );
    ui->serverTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    connect( &m_installer, &RenxInstaller::percentDownloaded,
             &m_downloadProgress, &DownloadDialog::downloadPercentageUpdated );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh(){
    QNetworkRequest req;
    req.setUrl( QUrl( "https://serverlist.ren-x.com/servers.jsp?id=launcher HTTP/1.1" ) );
    req.setRawHeader( "User-Agent:", "RenX-Launcher (0.84)" );

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            LOG4CXX_ERROR( logger, "Unable to download servers: " << reply->errorString().toStdString() );
            return;
        }

        QVector<ServerInformation> info;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

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
    //WINEPREFIX=~/.wine-renx/ winetricks d9vk040 corefonts vcrun2008 vcrun2010 xact win7 dotnet452
    //winetricks corefonts vcrun2008 vcrun2010 xact xact_x64 d3dx9 d3dx9_43 msxml3 dotnet452 win7
}
