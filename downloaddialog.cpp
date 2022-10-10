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

void DownloadDialog::downloadPercentageUpdated( double percentage ){
    int val = percentage;
    ui->progressBar->setValue( val );
}

void DownloadDialog::totalDownlaodPercentageUpdate( double percent ){
    int val = percent;
    ui->totalProgessbar->setValue( val );
}

void DownloadDialog::numFilesDownloadProgress( int numFilesDl, int totalNumFiles ){
    ui->numFilesDl->setText( QString( "%1/%2").arg( numFilesDl ).arg( totalNumFiles ) );
}
