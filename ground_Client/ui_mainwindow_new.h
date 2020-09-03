/********************************************************************************
** Form generated from reading UI file 'mainwindow_new.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_NEW_H
#define UI_MAINWINDOW_NEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow_New
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow_New)
    {
        if (MainWindow_New->objectName().isEmpty())
            MainWindow_New->setObjectName(QStringLiteral("MainWindow_New"));
        MainWindow_New->resize(1334, 759);
        QIcon icon;
        icon.addFile(QStringLiteral(":/plane.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow_New->setWindowIcon(icon);
        MainWindow_New->setAutoFillBackground(false);
        centralwidget = new QWidget(MainWindow_New);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        MainWindow_New->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow_New);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1334, 26));
        MainWindow_New->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow_New);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow_New->setStatusBar(statusbar);

        retranslateUi(MainWindow_New);

        QMetaObject::connectSlotsByName(MainWindow_New);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow_New)
    {
        MainWindow_New->setWindowTitle(QApplication::translate("MainWindow_New", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow_New: public Ui_MainWindow_New {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_NEW_H
