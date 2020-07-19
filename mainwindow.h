#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

#include "serverlistmodel.h"
#include "releaseinformation.h"

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

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager m_network;
    ServerListModel m_model;
    ReleaseInformation m_releaseInfo;
};

#endif // MAINWINDOW_H
