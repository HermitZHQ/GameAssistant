/********************************************************************************
** Form generated from reading UI file 'player.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYER_H
#define UI_PLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerUI
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *PlayerUI)
    {
        if (PlayerUI->objectName().isEmpty())
            PlayerUI->setObjectName(QString::fromUtf8("PlayerUI"));
        PlayerUI->resize(400, 539);
        pushButton = new QPushButton(PlayerUI);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(120, 30, 121, 28));
        pushButton_2 = new QPushButton(PlayerUI);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(140, 80, 93, 28));

        retranslateUi(PlayerUI);
        QObject::connect(pushButton, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnA4Reward()));

        QMetaObject::connectSlotsByName(PlayerUI);
    } // setupUi

    void retranslateUi(QWidget *PlayerUI)
    {
        PlayerUI->setWindowTitle(QApplication::translate("PlayerUI", "Form", nullptr));
        pushButton->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213A4\350\265\217\351\207\221\346\214\202\346\234\272", nullptr));
        pushButton_2->setText(QApplication::translate("PlayerUI", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerUI: public Ui_PlayerUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYER_H
