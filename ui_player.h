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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerUI
{
public:
    QPushButton *pushButton_2;
    QListWidget *list_tip;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_7;
    QComboBox *cmb_sim;
    QTextEdit *textEdit;
    QPushButton *btn_daily_2;
    QPushButton *pushButton;
    QLabel *label_5;
    QComboBox *cmb_delegate;
    QPushButton *btn_recruit;
    QLabel *label_4;
    QComboBox *cmb_reward;
    QComboBox *cmb_specific;
    QLabel *label_3;
    QComboBox *cmb_dev;
    QPushButton *btn_Dev;
    QPushButton *btn_solo;
    QPushButton *btn_daily;
    QLabel *label_6;
    QPushButton *btn_startAuto;
    QCheckBox *chk_reward;

    void setupUi(QWidget *PlayerUI)
    {
        if (PlayerUI->objectName().isEmpty())
            PlayerUI->setObjectName(QString::fromUtf8("PlayerUI"));
        PlayerUI->resize(631, 653);
        pushButton_2 = new QPushButton(PlayerUI);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(350, 530, 271, 51));
        list_tip = new QListWidget(PlayerUI);
        list_tip->setObjectName(QString::fromUtf8("list_tip"));
        list_tip->setGeometry(QRect(350, 100, 271, 421));
        verticalLayoutWidget_2 = new QWidget(PlayerUI);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(350, 10, 271, 80));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(verticalLayoutWidget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_7->setFont(font);

        verticalLayout_2->addWidget(label_7);

        cmb_sim = new QComboBox(verticalLayoutWidget_2);
        cmb_sim->addItem(QString());
        cmb_sim->addItem(QString());
        cmb_sim->setObjectName(QString::fromUtf8("cmb_sim"));
        cmb_sim->setLayoutDirection(Qt::LeftToRight);
        cmb_sim->setAutoFillBackground(true);
        cmb_sim->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        verticalLayout_2->addWidget(cmb_sim);

        textEdit = new QTextEdit(PlayerUI);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 413, 329, 113));
        btn_daily_2 = new QPushButton(PlayerUI);
        btn_daily_2->setObjectName(QString::fromUtf8("btn_daily_2"));
        btn_daily_2->setGeometry(QRect(10, 248, 329, 28));
        pushButton = new QPushButton(PlayerUI);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 46, 329, 28));
        label_5 = new QLabel(PlayerUI);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 281, 329, 15));
        cmb_delegate = new QComboBox(PlayerUI);
        cmb_delegate->addItem(QString());
        cmb_delegate->setObjectName(QString::fromUtf8("cmb_delegate"));
        cmb_delegate->setGeometry(QRect(10, 222, 60, 21));
        cmb_delegate->setLayoutDirection(Qt::LeftToRight);
        cmb_delegate->setAutoFillBackground(true);
        cmb_delegate->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        btn_recruit = new QPushButton(PlayerUI);
        btn_recruit->setObjectName(QString::fromUtf8("btn_recruit"));
        btn_recruit->setGeometry(QRect(10, 380, 329, 28));
        label_4 = new QLabel(PlayerUI);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 202, 329, 15));
        cmb_reward = new QComboBox(PlayerUI);
        cmb_reward->addItem(QString());
        cmb_reward->setObjectName(QString::fromUtf8("cmb_reward"));
        cmb_reward->setGeometry(QRect(10, 20, 81, 21));
        cmb_reward->setLayoutDirection(Qt::LeftToRight);
        cmb_reward->setAutoFillBackground(true);
        cmb_reward->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        cmb_specific = new QComboBox(PlayerUI);
        cmb_specific->addItem(QString());
        cmb_specific->addItem(QString());
        cmb_specific->addItem(QString());
        cmb_specific->addItem(QString());
        cmb_specific->addItem(QString());
        cmb_specific->addItem(QString());
        cmb_specific->setObjectName(QString::fromUtf8("cmb_specific"));
        cmb_specific->setGeometry(QRect(10, 531, 249, 21));
        cmb_specific->setLayoutDirection(Qt::LeftToRight);
        cmb_specific->setAutoFillBackground(true);
        cmb_specific->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        label_3 = new QLabel(PlayerUI);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 149, 329, 15));
        cmb_dev = new QComboBox(PlayerUI);
        cmb_dev->addItem(QString());
        cmb_dev->addItem(QString());
        cmb_dev->addItem(QString());
        cmb_dev->setObjectName(QString::fromUtf8("cmb_dev"));
        cmb_dev->setGeometry(QRect(10, 301, 106, 21));
        cmb_dev->setLayoutDirection(Qt::LeftToRight);
        cmb_dev->setAutoFillBackground(true);
        cmb_dev->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        btn_Dev = new QPushButton(PlayerUI);
        btn_Dev->setObjectName(QString::fromUtf8("btn_Dev"));
        btn_Dev->setGeometry(QRect(10, 327, 329, 28));
        btn_solo = new QPushButton(PlayerUI);
        btn_solo->setObjectName(QString::fromUtf8("btn_solo"));
        btn_solo->setGeometry(QRect(10, 557, 329, 28));
        btn_daily = new QPushButton(PlayerUI);
        btn_daily->setObjectName(QString::fromUtf8("btn_daily"));
        btn_daily->setGeometry(QRect(10, 169, 329, 28));
        label_6 = new QLabel(PlayerUI);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 360, 329, 15));
        btn_startAuto = new QPushButton(PlayerUI);
        btn_startAuto->setObjectName(QString::fromUtf8("btn_startAuto"));
        btn_startAuto->setGeometry(QRect(210, 590, 271, 51));
        chk_reward = new QCheckBox(PlayerUI);
        chk_reward->setObjectName(QString::fromUtf8("chk_reward"));
        chk_reward->setGeometry(QRect(100, 20, 121, 19));
        chk_reward->setChecked(true);

        retranslateUi(PlayerUI);
        QObject::connect(pushButton, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnA4Reward()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnStop()));
        QObject::connect(btn_daily, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnDaily()));
        QObject::connect(btn_daily_2, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnDelegate()));
        QObject::connect(btn_Dev, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnDev()));
        QObject::connect(btn_recruit, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnRecruit()));
        QObject::connect(btn_solo, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnSpecific()));
        QObject::connect(btn_startAuto, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnStartAuto()));

        QMetaObject::connectSlotsByName(PlayerUI);
    } // setupUi

    void retranslateUi(QWidget *PlayerUI)
    {
        PlayerUI->setWindowTitle(QApplication::translate("PlayerUI", "Form", nullptr));
        pushButton_2->setText(QApplication::translate("PlayerUI", "\345\201\234\346\255\242\350\204\232\346\234\254", nullptr));
        label_7->setText(QApplication::translate("PlayerUI", "\346\250\241\346\213\237\345\231\250\351\200\211\346\213\251\357\274\210\346\224\257\346\214\201\351\233\267\347\224\265\357\274\214\346\234\250\346\234\250\357\274\211", nullptr));
        cmb_sim->setItemText(0, QApplication::translate("PlayerUI", "\351\233\267\347\224\265", nullptr));
        cmb_sim->setItemText(1, QApplication::translate("PlayerUI", "MuMu\357\274\210\346\234\250\346\234\250\357\274\211", nullptr));

        textEdit->setHtml(QApplication::translate("PlayerUI", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">(7)\346\211\213\345\212\250\351\200\211\346\213\251\345\205\263\345\215\241\357\274\214\347\224\261\344\272\216\346\237\220\344\272\233\345\205\263\345\215\241\347\211\271\345\276\201\350\277\207\344\272\216\347\233\270\344\274\274\357\274\214\345\256\271\346\230\223\345\207\272\347\216\260\350\257\206\345\210\253\351\224\231\350\257\257\357\274\214\350\277\230\346\234\211\344\270\252\345\210\253\345\205\263\345\215\241\346\234\211\346\254\241\346\225\260\351\231\220\345\210\266\357\274\214\346\257\224\345\246\202\345\211\247\346\203"
                        "\205\345\211\257\346\234\254\344\273\245\345\217\212\346\231\256\351\200\232\345\211\257\346\234\254\357\274\214</span><span style=\" font-weight:600; text-decoration: underline;\">\345\217\257\344\273\245\345\234\250\350\277\233\345\205\245\345\205\263\345\215\241\345\220\216\346\211\213\345\212\250\351\200\211\346\213\251\345\257\271\345\272\224\350\204\232\346\234\254\346\211\247\350\241\214\357\274\214\345\217\252\344\274\232\346\211\247\350\241\214\344\270\200\346\254\241\357\274\214\345\217\257\345\276\252\347\216\257\347\232\204\345\205\263\345\215\241\345\234\250\345\274\200\345\247\213\344\273\273\345\212\241\345\244\204\346\211\247\350\241\214\345\215\263\345\217\257</span></p></body></html>", nullptr));
        btn_daily_2->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\345\247\224\346\211\230", nullptr));
        pushButton->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\350\265\217\351\207\221\346\214\202\346\234\272", nullptr));
        label_5->setText(QApplication::translate("PlayerUI", "(5)\350\207\252\345\212\250\345\274\200\345\217\221\357\274\21416\346\254\241\357\274\214\345\234\250\345\244\247\345\216\205\347\225\214\351\235\242\346\211\247\350\241\214", nullptr));
        cmb_delegate->setItemText(0, QApplication::translate("PlayerUI", "\346\234\200\346\205\242", nullptr));

        btn_recruit->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\346\213\233\345\213\237", nullptr));
        label_4->setText(QApplication::translate("PlayerUI", "(4)\350\207\252\345\212\250\345\247\224\346\211\230\357\274\214\345\234\250\345\244\247\345\216\205\347\225\214\351\235\242\346\211\247\350\241\214", nullptr));
        cmb_reward->setItemText(0, QApplication::translate("PlayerUI", "A4", nullptr));

        cmb_specific->setItemText(0, QApplication::translate("PlayerUI", "Nefud-1E/2E", nullptr));
        cmb_specific->setItemText(1, QApplication::translate("PlayerUI", "Hecate-1E", nullptr));
        cmb_specific->setItemText(2, QApplication::translate("PlayerUI", "\350\226\207\346\254\247\350\225\276\347\211\271\345\211\257\346\234\25450\357\274\210\345\217\257\345\276\252\347\216\257\357\274\211", nullptr));
        cmb_specific->setItemText(3, QApplication::translate("PlayerUI", "\350\226\207\346\254\247\350\225\276\347\211\271\345\211\257\346\234\25460\357\274\210\345\217\257\345\276\252\347\216\257\357\274\211", nullptr));
        cmb_specific->setItemText(4, QApplication::translate("PlayerUI", "\344\274\212\350\212\231\347\220\263\345\211\257\346\234\25450\357\274\210\347\254\254\345\233\233\347\253\240\357\274\214\345\217\257\345\276\252\347\216\257\357\274\211", nullptr));
        cmb_specific->setItemText(5, QApplication::translate("PlayerUI", "\344\274\212\350\212\231\347\220\263\345\211\257\346\234\25460\357\274\210\347\254\254\344\272\224\347\253\240\357\274\214\345\217\257\345\276\252\347\216\257\357\274\211", nullptr));

        label_3->setText(QApplication::translate("PlayerUI", "(3)\345\234\250\344\270\273\346\210\230\345\234\272\357\274\210\345\217\257\344\273\245\347\234\213\345\210\260A1-4\357\274\211\347\202\271\345\207\273\345\274\200\345\247\213", nullptr));
        cmb_dev->setItemText(0, QApplication::translate("PlayerUI", "\351\231\220\346\227\266\345\274\200\345\217\221", nullptr));
        cmb_dev->setItemText(1, QApplication::translate("PlayerUI", "\346\231\256\351\200\232\345\274\200\345\217\22110", nullptr));
        cmb_dev->setItemText(2, QApplication::translate("PlayerUI", "\346\231\256\351\200\232\345\274\200\345\217\22120", nullptr));

        btn_Dev->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\345\274\200\345\217\221", nullptr));
        btn_solo->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\347\213\254\347\253\213\345\205\263\345\215\241\345\215\225\346\254\241\350\277\220\350\241\214", nullptr));
        btn_daily->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\346\227\245\345\270\270\346\214\202\346\234\272", nullptr));
        label_6->setText(QApplication::translate("PlayerUI", "(6)\350\207\252\345\212\250\346\213\233\345\213\237\357\274\2144\346\254\241\357\274\214\345\234\250\345\244\247\345\216\205\347\225\214\351\235\242\346\211\247\350\241\214", nullptr));
        btn_startAuto->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\346\214\202\346\234\272", nullptr));
        chk_reward->setText(QApplication::translate("PlayerUI", "\350\207\252\345\212\250\350\265\217\351\207\221\346\214\202\346\234\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerUI: public Ui_PlayerUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYER_H
