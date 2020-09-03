/********************************************************************************
** Form generated from reading UI file 'menuwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENUWIDGET_H
#define UI_MENUWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MenuWidget
{
public:
    QPushButton *media;
    QPushButton *data;
    QPushButton *aboutButton;
    QPushButton *showWeb;

    void setupUi(QWidget *MenuWidget)
    {
        if (MenuWidget->objectName().isEmpty())
            MenuWidget->setObjectName(QStringLiteral("MenuWidget"));
        MenuWidget->resize(195, 639);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(0, 170, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        MenuWidget->setPalette(palette);
        MenuWidget->setStyleSheet(QStringLiteral(""));
        media = new QPushButton(MenuWidget);
        media->setObjectName(QStringLiteral("media"));
        media->setGeometry(QRect(30, 120, 161, 51));
        QFont font;
        font.setFamily(QStringLiteral("Agency FB"));
        font.setPointSize(12);
        media->setFont(font);
        media->setStyleSheet(QStringLiteral(""));
        data = new QPushButton(MenuWidget);
        data->setObjectName(QStringLiteral("data"));
        data->setGeometry(QRect(30, 200, 161, 51));
        data->setFont(font);
        data->setStyleSheet(QStringLiteral(""));
        aboutButton = new QPushButton(MenuWidget);
        aboutButton->setObjectName(QStringLiteral("aboutButton"));
        aboutButton->setGeometry(QRect(30, 360, 161, 51));
        aboutButton->setFont(font);
        aboutButton->setStyleSheet(QStringLiteral(""));
        showWeb = new QPushButton(MenuWidget);
        showWeb->setObjectName(QStringLiteral("showWeb"));
        showWeb->setGeometry(QRect(30, 280, 161, 51));
        showWeb->setFont(font);
        showWeb->setStyleSheet(QStringLiteral(""));

        retranslateUi(MenuWidget);

        QMetaObject::connectSlotsByName(MenuWidget);
    } // setupUi

    void retranslateUi(QWidget *MenuWidget)
    {
        MenuWidget->setWindowTitle(QApplication::translate("MenuWidget", "Form", nullptr));
        media->setText(QApplication::translate("MenuWidget", "\351\237\263\350\247\206\351\242\221\351\200\232\350\257\235", nullptr));
        data->setText(QApplication::translate("MenuWidget", "\346\225\260\346\215\256\344\272\244\344\272\222", nullptr));
        aboutButton->setText(QApplication::translate("MenuWidget", "\345\205\263\344\272\216", nullptr));
        showWeb->setText(QApplication::translate("MenuWidget", "\351\243\236\350\241\214\350\275\250\350\277\271", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MenuWidget: public Ui_MenuWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENUWIDGET_H
