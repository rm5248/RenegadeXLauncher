#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class DownloadDialog;
}

class DownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(QWidget *parent = nullptr);
    ~DownloadDialog();

public slots:
    void validationProgress( QString currentFile, double percent, int currentNum, int totalNumberFiles );
    void fileDownloadProgress( QString currentFile, double percent, int currentDownloadNum, int totalNumToDownload );
    void totalProgress( int percent, int currentNumber, int maxNumber  );
    void installationCompleted();

private:
    Ui::DownloadDialog *ui;
};

#endif // DOWNLOADDIALOG_H
