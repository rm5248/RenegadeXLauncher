#ifndef VALIDATIONDIALOG_H
#define VALIDATIONDIALOG_H

#include <QDialog>
#include <QVector>
#include "json-objects/instructionentry.h"

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

private:
    Ui::ValidationDialog *ui;
    QVector<InstructionEntry> m_installEntries;
};

#endif // VALIDATIONDIALOG_H
