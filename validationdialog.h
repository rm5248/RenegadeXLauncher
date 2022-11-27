#ifndef VALIDATIONDIALOG_H
#define VALIDATIONDIALOG_H

#include <QDialog>
#include <QVector>
#include <QThread>

#include "json-objects/instructionentry.h"
#include "filevalidator.h"

namespace Ui {
class ValidationDialog;
}

class ValidationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ValidationDialog(QWidget *parent = nullptr);
    ~ValidationDialog();

    void setValidationData( QVector<InstructionEntry> );
    void checksumCompleted( bool, QString );

private slots:
    void on_goodRadio_clicked();

    void on_badRadio_clicked();

    void on_allRadio_clicked();

private:
    void resortFiles();

private:
    Ui::ValidationDialog *ui;
    QVector<InstructionEntry> m_installEntries;
    QThread m_checksumThread;
    int m_totalFiles;
    int m_hashedFiles;
    int m_filesGood;
    int m_filesBad;
};

#endif // VALIDATIONDIALOG_H
