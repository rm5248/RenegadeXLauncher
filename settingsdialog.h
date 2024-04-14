#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void settingsAccepted();
    void wineOutput();
    void wineConfigFinished( int exitCode, QProcess::ExitStatus status );

    void on_launchWinecfg_clicked();

    void on_launchWinetricks_clicked();

    void on_autoDetectSteam_clicked();

    void on_browseSteam_clicked();

private:
    Ui::SettingsDialog *ui;
    QProcess m_winecfg;
    QProcess m_winetricks;
};

#endif // SETTINGSDIALOG_H
