/********************************************************************************
** Form generated from reading UI file 'mediawidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEDIAWIDGET_H
#define UI_MEDIAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MediaWidget
{
public:
    QLabel *label;

    void setupUi(QWidget *MediaWidget)
    {
        if (MediaWidget->objectName().isEmpty())
            MediaWidget->setObjectName(QStringLiteral("MediaWidget"));
        MediaWidget->resize(810, 562);
        QFont font;
        font.setPointSize(11);
        MediaWidget->setFont(font);
        label = new QLabel(MediaWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(60, 60, 121, 51));

        retranslateUi(MediaWidget);

        QMetaObject::connectSlotsByName(MediaWidget);
    } // setupUi

    void retranslateUi(QWidget *MediaWidget)
    {
        MediaWidget->setWindowTitle(QApplication::translate("MediaWidget", "Form", nullptr));
        label->setText(QApplication::translate("MediaWidget", "\350\257\267\350\276\223\345\205\245\345\257\271\346\226\271ID:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MediaWidget: public Ui_MediaWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEDIAWIDGET_H
