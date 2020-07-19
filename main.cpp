#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QTimer::singleShot( 0, &w, &MainWindow::refresh );
    QTimer::singleShot( 0, &w, &MainWindow::checkForUpdates );

    return a.exec();
}
