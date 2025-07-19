#include "downloaddialog.h"
#include "ui_downloaddialog.h"

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::validationProgress( QString currentFile, double percent, int currentNum, int totalNumberFiles ){
    ui->validating_info->setText( QString( "%1/%2").arg( currentNum ).arg( totalNumberFiles ) );
    ui->validatingProgress->setValue( percent );
}

void DownloadDialog::fileDownloadProgress( QString currentFile, double percent, int currentDownloadNum, int totalNumToDownload ){
    ui->downloading_info->setText( QString( "%1/%2").arg( currentDownloadNum ).arg( totalNumToDownload ) );
    ui->downloadProgress->setValue( percent );
}

void DownloadDialog::totalProgress( int percent, int currentNumber, int maxNumber  ){
    ui->totalProgessbar->setValue( percent );
    ui->totalFilesValidation->setText( QString( "%1/%2").arg( currentNumber ).arg( maxNumber ) );
}

void DownloadDialog::installationCompleted(){

}
