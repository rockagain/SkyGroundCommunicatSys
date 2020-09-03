/********************************************************************************
** Form generated from reading UI file 'senddatawidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SENDDATAWIDGET_H
#define UI_SENDDATAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_sendDataWidget
{
public:
    QTabWidget *tabWidget;
    QWidget *AIMMS30;
    QLabel *label_2;
    QLabel *label_7;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_5;
    QLineEdit *lineEdit;
    QLabel *label_3;
    QLineEdit *lineEdit_6;
    QLabel *label;
    QLabel *label_8;
    QLineEdit *lineEdit_7;
    QWidget *tab_8;
    QWidget *tab_9;
    QWidget *tab_10;
    QWidget *tab_11;
    QWidget *tab_12;
    QWidget *tab_13;
    QWidget *tab_14;
    QWidget *tab_6;
    QWidget *tab_7;
    QWidget *tab_4;
    QWidget *tab_5;
    QWidget *tab_2;
    QWidget *tab_3;

    void setupUi(QWidget *sendDataWidget)
    {
        if (sendDataWidget->objectName().isEmpty())
            sendDataWidget->setObjectName(QStringLiteral("sendDataWidget"));
        sendDataWidget->resize(998, 536);
        tabWidget = new QTabWidget(sendDataWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(40, 30, 761, 511));
        AIMMS30 = new QWidget();
        AIMMS30->setObjectName(QStringLiteral("AIMMS30"));
        label_2 = new QLabel(AIMMS30);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(150, 100, 72, 16));
        label_7 = new QLabel(AIMMS30);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(410, 100, 72, 16));
        lineEdit_2 = new QLineEdit(AIMMS30);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(200, 100, 61, 21));
        lineEdit_5 = new QLineEdit(AIMMS30);
        lineEdit_5->setObjectName(QStringLiteral("lineEdit_5"));
        lineEdit_5->setGeometry(QRect(330, 100, 61, 21));
        lineEdit = new QLineEdit(AIMMS30);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(80, 100, 61, 21));
        label_3 = new QLabel(AIMMS30);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(270, 100, 72, 16));
        lineEdit_6 = new QLineEdit(AIMMS30);
        lineEdit_6->setObjectName(QStringLiteral("lineEdit_6"));
        lineEdit_6->setGeometry(QRect(460, 100, 61, 21));
        label = new QLabel(AIMMS30);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 100, 72, 16));
        label_8 = new QLabel(AIMMS30);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(530, 100, 72, 16));
        lineEdit_7 = new QLineEdit(AIMMS30);
        lineEdit_7->setObjectName(QStringLiteral("lineEdit_7"));
        lineEdit_7->setGeometry(QRect(570, 100, 61, 21));
        tabWidget->addTab(AIMMS30, QString());
        tab_8 = new QWidget();
        tab_8->setObjectName(QStringLiteral("tab_8"));
        tabWidget->addTab(tab_8, QString());
        tab_9 = new QWidget();
        tab_9->setObjectName(QStringLiteral("tab_9"));
        tabWidget->addTab(tab_9, QString());
        tab_10 = new QWidget();
        tab_10->setObjectName(QStringLiteral("tab_10"));
        tabWidget->addTab(tab_10, QString());
        tab_11 = new QWidget();
        tab_11->setObjectName(QStringLiteral("tab_11"));
        tabWidget->addTab(tab_11, QString());
        tab_12 = new QWidget();
        tab_12->setObjectName(QStringLiteral("tab_12"));
        tabWidget->addTab(tab_12, QString());
        tab_13 = new QWidget();
        tab_13->setObjectName(QStringLiteral("tab_13"));
        tabWidget->addTab(tab_13, QString());
        tab_14 = new QWidget();
        tab_14->setObjectName(QStringLiteral("tab_14"));
        tabWidget->addTab(tab_14, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        tabWidget->addTab(tab_6, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QStringLiteral("tab_7"));
        tabWidget->addTab(tab_7, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        tabWidget->addTab(tab_5, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        tabWidget->addTab(tab_3, QString());

        retranslateUi(sendDataWidget);

        tabWidget->setCurrentIndex(12);


        QMetaObject::connectSlotsByName(sendDataWidget);
    } // setupUi

    void retranslateUi(QWidget *sendDataWidget)
    {
        sendDataWidget->setWindowTitle(QApplication::translate("sendDataWidget", "Form", nullptr));
        label_2->setText(QApplication::translate("sendDataWidget", "\346\271\277\345\272\246", nullptr));
        label_7->setText(QApplication::translate("sendDataWidget", "\351\243\216\351\200\237", nullptr));
        label_3->setText(QApplication::translate("sendDataWidget", "\345\244\247\346\260\224\345\216\213", nullptr));
        lineEdit_6->setText(QString());
        label->setText(QApplication::translate("sendDataWidget", "\346\270\251\345\272\246", nullptr));
        label_8->setText(QApplication::translate("sendDataWidget", "\351\243\216\345\220\221 ", nullptr));
        lineEdit_7->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(AIMMS30), QApplication::translate("sendDataWidget", "AIMMS-30\346\240\207\345\207\206\346\260\224\350\261\241\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_8), QApplication::translate("sendDataWidget", "AIMMS-30\351\243\236\346\234\272\347\212\266\346\200\201\345\214\205\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_9), QApplication::translate("sendDataWidget", "FCDP\346\225\260\346\215\256\345\270\247", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_10), QApplication::translate("sendDataWidget", "D/XAS\344\277\241\346\201\257\346\225\260\346\215\256\345\270\247", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_11), QApplication::translate("sendDataWidget", "LWC/TWC\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_12), QApplication::translate("sendDataWidget", "PCASP\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_13), QApplication::translate("sendDataWidget", "LWC-100\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_14), QApplication::translate("sendDataWidget", "CIP\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("sendDataWidget", "CDP\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_7), QApplication::translate("sendDataWidget", "CPI\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("sendDataWidget", "2D-S\344\277\241\346\201\257\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("sendDataWidget", "\351\243\236\346\234\272\344\275\215\347\275\256\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("sendDataWidget", "\351\243\236\346\234\272\347\212\266\346\200\201\346\225\260\346\215\256\345\270\247", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("sendDataWidget", "\345\274\200\351\200\232\345\214\227\346\226\227\346\225\260\346\215\256\345\270\247", nullptr));
    } // retranslateUi

};

namespace Ui {
    class sendDataWidget: public Ui_sendDataWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SENDDATAWIDGET_H
