#include "checksumdisplay.h"
#include "ui_checksumdisplay.h"

ChecksumDisplay::ChecksumDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChecksumDisplay),
    m_state(ChecksumState::Incomplete)
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
        m_state = ChecksumState::Good;
        emit checksumStateChanged();
    }else{
        ui->calculatedChecksum->setStyleSheet( "color: rgb(233, 0, 0);" );
        m_state = ChecksumState::Bad;
        emit checksumStateChanged();
    }
}

ChecksumDisplay::ChecksumState ChecksumDisplay::checksumState() const{
    return m_state;
}
