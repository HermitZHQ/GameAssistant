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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerUI
{
public:
    QPushButton *pushButton_2;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPushButton *btn_nextStep;
    QLabel *label_2;
    QComboBox *cmb_reward;
    QPushButton *pushButton;
    QLabel *label_3;
    QPushButton *btn_daily;
    QLabel *label_4;
    QComboBox *cmb_delegate;
    QPushButton *btn_daily_2;
    QListWidget *list_tip;

    void setupUi(QWidget *PlayerUI)
    {
        if (PlayerUI->objectName().isEmpty())
            PlayerUI->setObjectName(QString::fromUtf8("PlayerUI"));
        PlayerUI->resize(405, 600);
        pushButton_2 = new QPushButton(PlayerUI);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(12, 539, 381, 51));
        verticalLayoutWidget = new QWidget(PlayerUI);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(40, 30, 315, 331));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        btn_nextStep = new QPushButton(verticalLayoutWidget);
        btn_nextStep->setObjectName(QString::fromUtf8("btn_nextStep"));

        verticalLayout->addWidget(btn_nextStep);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        cmb_reward = new QComboBox(verticalLayoutWidget);
        cmb_reward->addItem(QString());
        cmb_reward->setObjectName(QString::fromUtf8("cmb_reward"));
        cmb_reward->setLayoutDirection(Qt::LeftToRight);
        cmb_reward->setAutoFillBackground(true);
        cmb_reward->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        verticalLayout->addWidget(cmb_reward);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        btn_daily = new QPushButton(verticalLayoutWidget);
        btn_daily->setObjectName(QString::fromUtf8("btn_daily"));

        verticalLayout->addWidget(btn_daily);

        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        cmb_delegate = new QComboBox(verticalLayoutWidget);
        cmb_delegate->addItem(QString());
        cmb_delegate->setObjectName(QString::fromUtf8("cmb_delegate"));
        cmb_delegate->setLayoutDirection(Qt::LeftToRight);
        cmb_delegate->setAutoFillBackground(true);
        cmb_delegate->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        verticalLayout->addWidget(cmb_delegate);

        btn_daily_2 = new QPushButton(verticalLayoutWidget);
        btn_daily_2->setObjectName(QString::fromUtf8("btn_daily_2"));

        verticalLayout->addWidget(btn_daily_2);

        list_tip = new QListWidget(PlayerUI);
        list_tip->setObjectName(QString::fromUtf8("list_tip"));
        list_tip->setGeometry(QRect(10, 400, 381, 131));

        retranslateUi(PlayerUI);
        QObject::connect(pushButton, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnA4Reward()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnStop()));
        QObject::connect(btn_daily, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnDaily()));
        QObject::connect(btn_nextStep, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnNextStep()));
        QObject::connect(btn_daily_2, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnDelegate()));

        QMetaObject::connectSlotsByName(PlayerUI);
    } // setupUi

    void retranslateUi(QWidget *PlayerUI)
    {
        PlayerUI->setWindowTitle(QApplication::translate("PlayerUI", "Form", nullptr));
        pushButton_2->setText(QApplication::translate("PlayerUI", "\345\201\234\346\255\242\350\204\232\346\234\254", nullptr));
        label->setText(QApplication::translate("PlayerUI", "(1)\347\224\250\344\272\216\345\244\215\346\235\202\345\205\263\345\215\241\357\274\214\346\211\213\345\212\250\346\213\211\345\245\275\350\267\257\347\272\277\345\220\216\347\232\204\346\211\230\347\256\241", nullptr));
        btn_nextStep->setText(QApplication::translate("PlayerUI", "\344\270\213\344\270\200\346\255\245\345\222\214\345\274\200\346\210\230\350\276\205\345\212\251", nullptr));
        label_2->setText(QApplication::translate("PlayerUI", "(2)\350\277\233\345\205\245A(1-4)\346\210\230\345\234\272\345\220\216\347\202\271\345\207\273\345\274\200\345\247\213\357\274\214\347\233\256\345\211\215\345\217\252\346\224\257\346\214\201A4", nullptr));
        cmb_reward->setItemText(0, QApplication::translate("PlayerUI", "A4", nullptr));

        pushButton->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\350\265\217\351\207\221\346\214\202\346\234\272", nullptr));
        label_3->setText(QApplication::translate("PlayerUI", "(3)\345\234\250\344\270\273\346\210\230\345\234\272\357\274\210\345\217\257\344\273\245\347\234\213\345\210\260A1-4\357\274\211\347\202\271\345\207\273\345\274\200\345\247\213", nullptr));
        btn_daily->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\346\227\245\345\270\270\346\214\202\346\234\272", nullptr));
        label_4->setText(QApplication::translate("PlayerUI", "(4)\345\274\200\345\247\213\350\207\252\345\212\250\345\247\224\346\211\230", nullptr));
        cmb_delegate->setItemText(0, QApplication::translate("PlayerUI", "\346\234\200\346\205\242", nullptr));

        btn_daily_2->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\345\247\224\346\211\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerUI: public Ui_PlayerUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYER_H
