#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json-objects/releaseinformation.h"
#include "settingsdialog.h"
#include "json-objects/instructionentry.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->serverTable->setModel( &m_model );
    ui->serverTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh(){
    QNetworkRequest req;
    req.setUrl( QUrl( "https://serverlist.renegade-x.com/servers.jsp?id=launcher HTTP/1.1" ) );
    req.setRawHeader( "User-Agent:", "RenX-Launcher (0.84)" );

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            qDebug() << "Error: " << reply->errorString();
            return;
        }

        QVector<ServerInformation> info;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            qDebug() << "Bad JSON document";
            return;
        }

        if( !jsonDoc.isArray() ){
            qDebug() << "not array";
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
    req.setUrl( QUrl( "https://static.renegade-x.com/launcher_data/version/release.json" ) );

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            qDebug() << "Error: " << reply->errorString();
            return;
        }

        QVector<ServerInformation> info;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            qDebug() << "Bad JSON document";
            return;
        }

        ReleaseInformation ri( jsonDoc );

        m_releaseInfo = ri;

        qDebug() << "Latest game version is " << ri.gameInfo().version_number();
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
    QRandomGenerator randGen;
    int randomMirrorNumber = randGen.bounded( 0, mirrors.length() );
    GameInfo::MirrorInfo mirrorToUse = mirrors[ randomMirrorNumber ];

    req.setUrl( QUrl( mirrorToUse.url + "/" + m_releaseInfo.gameInfo().patch_path() + "/instructions.json" ) );

    QNetworkReply* reply = m_network.get( req );
    connect( reply, &QNetworkReply::finished, [reply,this](){
        reply->deleteLater();

        if( reply->error() != QNetworkReply::NoError ){
            qDebug() << "Error: " << reply->errorString();
            return;
        }

        QVector<ServerInformation> info;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll() );

        if( jsonDoc.isNull() ){
            qDebug() << "Bad JSON document";
            return;
        }

        QJsonArray installationArray = jsonDoc.array();
        QVector<InstructionEntry> instructions;
        for( QJsonValue obj : installationArray ){
            instructions.push_back( InstructionEntry( obj.toObject() ) );
        }
    });
}
