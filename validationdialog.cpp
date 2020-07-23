#include "validationdialog.h"
#include "ui_validationdialog.h"

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

}
