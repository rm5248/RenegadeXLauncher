#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

#include "mainwindow.h"
#include "ui_mainwindow.h"

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
