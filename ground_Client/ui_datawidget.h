/********************************************************************************
** Form generated from reading UI file 'datawidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAWIDGET_H
#define UI_DATAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dataWidget
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;

    void setupUi(QWidget *dataWidget)
    {
        if (dataWidget->objectName().isEmpty())
            dataWidget->setObjectName(QStringLiteral("dataWidget"));
        dataWidget->resize(1135, 696);
        QFont font;
        font.setPointSize(11);
        dataWidget->setFont(font);
        label = new QLabel(dataWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 20, 81, 21));
        label_2 = new QLabel(dataWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 60, 141, 21));
        label_2->setFont(font);
        label_3 = new QLabel(dataWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 90, 161, 21));
        label_5 = new QLabel(dataWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(370, 20, 161, 31));
        label_6 = new QLabel(dataWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(740, 20, 111, 31));
        label_7 = new QLabel(dataWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(380, 310, 121, 31));
        radioButton = new QRadioButton(dataWidget);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(500, 570, 61, 16));
        radioButton_2 = new QRadioButton(dataWidget);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        radioButton_2->setGeometry(QRect(560, 570, 71, 16));
        radioButton_3 = new QRadioButton(dataWidget);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));
        radioButton_3->setGeometry(QRect(630, 570, 61, 16));

        retranslateUi(dataWidget);

        QMetaObject::connectSlotsByName(dataWidget);
    } // setupUi

    void retranslateUi(QWidget *dataWidget)
    {
        dataWidget->setWindowTitle(QApplication::translate("dataWidget", "Form", nullptr));
        label->setText(QApplication::translate("dataWidget", "\347\237\255\346\266\210\346\201\257", nullptr));
        label_2->setText(QApplication::translate("dataWidget", "\350\257\267\350\276\223\345\205\245\351\200\232\350\256\257\345\217\267\347\240\201:", nullptr));
        label_3->setText(QApplication::translate("dataWidget", "\350\257\267\350\276\223\345\205\245\345\217\221\351\200\201\347\232\204\345\206\205\345\256\271:", nullptr));
        label_5->setText(QApplication::translate("dataWidget", "\346\224\266\345\210\260\347\232\204\347\237\255\344\277\241\346\201\257", nullptr));
        label_6->setText(QApplication::translate("dataWidget", "\345\217\221\351\200\201\347\232\204\347\237\255\344\277\241\346\201\257", nullptr));
        label_7->setText(QApplication::translate("dataWidget", "\345\205\266\344\273\226\346\214\207\344\273\244", nullptr));
        radioButton->setText(QApplication::translate("dataWidget", "1S", nullptr));
        radioButton_2->setText(QApplication::translate("dataWidget", "10S", nullptr));
        radioButton_3->setText(QApplication::translate("dataWidget", "60S", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dataWidget: public Ui_dataWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAWIDGET_H
