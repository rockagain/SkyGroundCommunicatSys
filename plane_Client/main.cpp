#include "mainwindow_new.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow_New w;
    w.show();
    w.autoFillBackground();
    return a.exec();
}
