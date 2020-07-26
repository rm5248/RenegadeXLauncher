#include "checksumdisplay.h"
#include "ui_checksumdisplay.h"

ChecksumDisplay::ChecksumDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChecksumDisplay)
{
    ui->setupUi(this);
}

ChecksumDisplay::~ChecksumDisplay()
{
    delete ui;
}

void ChecksumDisplay::setExpected(QString expected){
    ui->expectedChecksum->setText( expected );
}

void ChecksumDisplay::setFilename(QString filename){
    ui->fileLabel->setText( filename );
}

void ChecksumDisplay::checksumCalculationCompleted( bool success, QString calculated ){
    ui->calculatedChecksum->setText( calculated );
    if( success ){
        ui->calculatedChecksum->setStyleSheet( "color: rgb(0, 255, 0);" );
    }else{
        ui->calculatedChecksum->setStyleSheet( "color: rgb(233, 0, 0);" );
    }
}
