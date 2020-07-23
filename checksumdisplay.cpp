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
