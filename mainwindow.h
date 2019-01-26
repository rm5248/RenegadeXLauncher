#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

#include "serverlistmodel.h"

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

private slots:
    void on_refreshButton_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager m_network;
    ServerListModel m_model;
};

#endif // MAINWINDOW_H
