#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QSettings settings( "rm5248", "RenegadeXLauncher" );

    QString defaultWineLocation = "~/.wine";
    QString wineLocation = settings.value( "wineprefix", defaultWineLocation ).toString();
    ui->wineprefix->setText( wineLocation );

    connect( this, &QDialog::accepted,
             this, &SettingsDialog::settingsAccepted );
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::settingsAccepted(){
    QSettings settings( "rm5248", "RenegadeXLauncher" );
    settings.setValue( "wineprefix", ui->wineprefix->text() );
}
