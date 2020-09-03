/********************************************************************************
** Form generated from reading UI file 'streamplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STREAMPLAYER_H
#define UI_STREAMPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StreamPlayer
{
public:

    void setupUi(QWidget *StreamPlayer)
    {
        if (StreamPlayer->objectName().isEmpty())
            StreamPlayer->setObjectName(QStringLiteral("StreamPlayer"));
        StreamPlayer->resize(400, 300);

        retranslateUi(StreamPlayer);

        QMetaObject::connectSlotsByName(StreamPlayer);
    } // setupUi

    void retranslateUi(QWidget *StreamPlayer)
    {
        StreamPlayer->setWindowTitle(QApplication::translate("StreamPlayer", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class StreamPlayer: public Ui_StreamPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STREAMPLAYER_H
