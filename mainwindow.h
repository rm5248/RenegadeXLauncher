#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QProcess>

#include "serverlistmodel.h"
#include "json-objects/releaseinformation.h"
#include "validationdialog.h"
#include "renxinstaller.h"
#include "downloaddialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void refresh();
    void checkForUpdates();

private slots:
    void on_refreshButton_clicked();

    void on_actionSettings_triggered();

    void on_actionExit_triggered();

    void on_actionValidate_Install_triggered();

    void on_actionInstall_triggered();

    void on_actionLaunch_Game_triggered();

    void on_actionPerform_Winetricks_triggered();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager m_network;
    ServerListModel m_model;
    ReleaseInformation m_releaseInfo;
    ValidationDialog m_validationDialog;
    RenxInstaller m_installer;
    DownloadDialog m_downloadProgress;
    QProcess m_game;
    QProcess m_winetricks;
};

#endif // MAINWINDOW_H
