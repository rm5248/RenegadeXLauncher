#include <QVBoxLayout>

#include "validationdialog.h"
#include "ui_validationdialog.h"

#include "checksumdisplay.h"
#include "filevalidator.h"
#include "renx-config.h"

#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>

static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( "com.rm5248.RenegadeXLauncher.ValidationDialog" );

ValidationDialog::ValidationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValidationDialog)
{
    ui->setupUi(this);
}

ValidationDialog::~ValidationDialog()
{
    delete ui;
}

void ValidationDialog::setValidationData( QVector<InstructionEntry> entries ){
    QString basePath = renx_baseInstallPath();
    QWidget* toInsert = new QWidget( this );
    QVBoxLayout* insertLayout = new QVBoxLayout( toInsert );
    QVector<FileValidator*> filesToValidate;

    m_filesBad = 0;
    m_filesGood = 0;
    m_hashedFiles = 0;
    m_totalFiles = entries.size();
    ui->progressBar->setValue( 0 );
    ui->totalFiles->setText( QString::number( m_totalFiles ) );

    for( InstructionEntry instr : entries ){
        ChecksumDisplay* newChecksumDisplay = new ChecksumDisplay( toInsert );
        FileValidator* validator = new FileValidator( basePath + instr.path(),
                                                      instr.newHash() );

        filesToValidate.push_back( validator );
        validator->moveToThread( &m_checksumThread );

        newChecksumDisplay->setFilename( instr.path() );
        newChecksumDisplay->setExpected( instr.newHash() );

        connect( validator, &FileValidator::checksumCompleted,
                 newChecksumDisplay, &ChecksumDisplay::checksumCalculationCompleted );
        connect( validator, &FileValidator::checksumCompleted,
                 this, &ValidationDialog::checksumCompleted );
        connect( validator, &FileValidator::checksumCompleted,
                 [validator](){
            validator->deleteLater();
        }
                 );
        connect( &m_checksumThread, &QThread::started,
                 validator, &FileValidator::startChecksum );

        insertLayout->insertWidget( 0, newChecksumDisplay );
    }

    m_checksumThread.start();

    ui->scrollArea->setWidget( toInsert );
}

void ValidationDialog::checksumCompleted(bool good, QString){
    m_hashedFiles++;

    if( good ){
        m_filesGood++;
    }else{
        m_filesBad++;
    }

    double value = (static_cast<double>( m_hashedFiles ) / m_totalFiles) * 100.0;
    ui->progressBar->setValue( value );
    ui->filesBad->setText( QString::number( m_filesBad ) );
    ui->filesGood->setText( QString::number( m_filesGood ) );

    if( m_hashedFiles == m_totalFiles ){
        m_checksumThread.quit();
    }

    resortFiles();
}

void ValidationDialog::on_goodRadio_clicked()
{
    resortFiles();
}

void ValidationDialog::resortFiles(){
    QWidget* parent = ui->scrollArea->widget();

    QList<ChecksumDisplay*> children = parent->findChildren<ChecksumDisplay*>();
    LOG4CXX_DEBUG( logger, "Found " << children.size() << " checksum children" );

    if( ui->allRadio->isChecked() ){
        for( ChecksumDisplay* disp : children ){
            disp->setVisible( true );
        }
    }else if( ui->badRadio->isChecked() ){
        for( ChecksumDisplay* disp : children ){
            if( disp->checksumState() == ChecksumDisplay::ChecksumState::Bad ){
                disp->setVisible(true);
            }else{
                disp->setVisible(false);
            }
        }
    }else if( ui->goodRadio->isChecked() ){
        for( ChecksumDisplay* disp : children ){
            if( disp->checksumState() == ChecksumDisplay::ChecksumState::Good ){
                disp->setVisible(true);
            }else{
                disp->setVisible(false);
            }
        }
    }
}

void ValidationDialog::on_badRadio_clicked()
{
    resortFiles();
}

void ValidationDialog::on_allRadio_clicked()
{
    resortFiles();
}
