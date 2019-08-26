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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
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
    QPushButton *btn_startAuto;
    QGroupBox *groupBox;
    QCheckBox *chk_delegate;
    QCheckBox *chk_dev;
    QComboBox *cmb_dev;
    QLabel *label_8;
    QLabel *label_5;
    QComboBox *cmb_delegate;
    QLineEdit *edt_delegateInterval;
    QLineEdit *edt_devInterval;
    QCheckBox *chk_recruit;
    QLabel *label_9;
    QLineEdit *edt_recruitInterval;
    QPushButton *btn_noneBattle_toggle;
    QGroupBox *groupBox_2;
    QCheckBox *chk_daily;
    QCheckBox *chk_reward;
    QLabel *label_6;
    QLineEdit *edt_emergencyInterval;
    QCheckBox *chk_emergency;
    QPushButton *btn_battle_toggle;
    QGroupBox *groupBox_3;
    QTextEdit *textEdit;
    QPushButton *btn_huodong1;
    QLabel *label_10;

    void setupUi(QWidget *PlayerUI)
    {
        if (PlayerUI->objectName().isEmpty())
            PlayerUI->setObjectName(QString::fromUtf8("PlayerUI"));
        PlayerUI->resize(631, 653);
        pushButton_2 = new QPushButton(PlayerUI);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(350, 596, 271, 51));
        list_tip = new QListWidget(PlayerUI);
        list_tip->setObjectName(QString::fromUtf8("list_tip"));
        list_tip->setGeometry(QRect(10, 390, 611, 201));
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

        btn_startAuto = new QPushButton(PlayerUI);
        btn_startAuto->setObjectName(QString::fromUtf8("btn_startAuto"));
        btn_startAuto->setGeometry(QRect(10, 596, 271, 51));
        groupBox = new QGroupBox(PlayerUI);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 170, 251, 211));
        chk_delegate = new QCheckBox(groupBox);
        chk_delegate->setObjectName(QString::fromUtf8("chk_delegate"));
        chk_delegate->setGeometry(QRect(10, 100, 91, 21));
        chk_delegate->setChecked(true);
        chk_dev = new QCheckBox(groupBox);
        chk_dev->setObjectName(QString::fromUtf8("chk_dev"));
        chk_dev->setGeometry(QRect(10, 42, 91, 21));
        chk_dev->setChecked(true);
        cmb_dev = new QComboBox(groupBox);
        cmb_dev->addItem(QString());
        cmb_dev->addItem(QString());
        cmb_dev->addItem(QString());
        cmb_dev->setObjectName(QString::fromUtf8("cmb_dev"));
        cmb_dev->setGeometry(QRect(10, 20, 86, 20));
        cmb_dev->setLayoutDirection(Qt::LeftToRight);
        cmb_dev->setAutoFillBackground(true);
        cmb_dev->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(140, 80, 61, 16));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(140, 23, 61, 16));
        cmb_delegate = new QComboBox(groupBox);
        cmb_delegate->addItem(QString());
        cmb_delegate->setObjectName(QString::fromUtf8("cmb_delegate"));
        cmb_delegate->setGeometry(QRect(10, 77, 81, 20));
        cmb_delegate->setLayoutDirection(Qt::LeftToRight);
        cmb_delegate->setAutoFillBackground(true);
        cmb_delegate->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        edt_delegateInterval = new QLineEdit(groupBox);
        edt_delegateInterval->setObjectName(QString::fromUtf8("edt_delegateInterval"));
        edt_delegateInterval->setGeometry(QRect(106, 77, 31, 21));
        edt_devInterval = new QLineEdit(groupBox);
        edt_devInterval->setObjectName(QString::fromUtf8("edt_devInterval"));
        edt_devInterval->setGeometry(QRect(106, 20, 31, 21));
        chk_recruit = new QCheckBox(groupBox);
        chk_recruit->setObjectName(QString::fromUtf8("chk_recruit"));
        chk_recruit->setGeometry(QRect(10, 140, 91, 21));
        chk_recruit->setChecked(true);
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(140, 143, 61, 16));
        edt_recruitInterval = new QLineEdit(groupBox);
        edt_recruitInterval->setObjectName(QString::fromUtf8("edt_recruitInterval"));
        edt_recruitInterval->setGeometry(QRect(106, 140, 31, 21));
        btn_noneBattle_toggle = new QPushButton(groupBox);
        btn_noneBattle_toggle->setObjectName(QString::fromUtf8("btn_noneBattle_toggle"));
        btn_noneBattle_toggle->setGeometry(QRect(180, 180, 71, 28));
        groupBox_2 = new QGroupBox(PlayerUI);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 10, 251, 151));
        chk_daily = new QCheckBox(groupBox_2);
        chk_daily->setObjectName(QString::fromUtf8("chk_daily"));
        chk_daily->setGeometry(QRect(10, 60, 201, 21));
        chk_daily->setChecked(true);
        chk_reward = new QCheckBox(groupBox_2);
        chk_reward->setObjectName(QString::fromUtf8("chk_reward"));
        chk_reward->setGeometry(QRect(10, 90, 201, 21));
        chk_reward->setChecked(true);
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(164, 33, 71, 16));
        edt_emergencyInterval = new QLineEdit(groupBox_2);
        edt_emergencyInterval->setObjectName(QString::fromUtf8("edt_emergencyInterval"));
        edt_emergencyInterval->setGeometry(QRect(130, 31, 31, 21));
        chk_emergency = new QCheckBox(groupBox_2);
        chk_emergency->setObjectName(QString::fromUtf8("chk_emergency"));
        chk_emergency->setGeometry(QRect(10, 30, 121, 21));
        chk_emergency->setChecked(true);
        btn_battle_toggle = new QPushButton(groupBox_2);
        btn_battle_toggle->setObjectName(QString::fromUtf8("btn_battle_toggle"));
        btn_battle_toggle->setGeometry(QRect(179, 120, 71, 28));
        groupBox_3 = new QGroupBox(PlayerUI);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(350, 90, 271, 201));
        textEdit = new QTextEdit(groupBox_3);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 20, 251, 61));
        btn_huodong1 = new QPushButton(groupBox_3);
        btn_huodong1->setObjectName(QString::fromUtf8("btn_huodong1"));
        btn_huodong1->setGeometry(QRect(10, 84, 131, 28));
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(150, 90, 111, 16));

        retranslateUi(PlayerUI);
        QObject::connect(pushButton_2, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnStop()));
        QObject::connect(btn_startAuto, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnStartAuto()));
        QObject::connect(btn_battle_toggle, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnToggleBattleOptions()));
        QObject::connect(btn_noneBattle_toggle, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnToggleNoneBattleOptions()));
        QObject::connect(btn_huodong1, SIGNAL(clicked()), PlayerUI, SLOT(OnBtnHuodong1()));

        QMetaObject::connectSlotsByName(PlayerUI);
    } // setupUi

    void retranslateUi(QWidget *PlayerUI)
    {
        PlayerUI->setWindowTitle(QApplication::translate("PlayerUI", "Form", nullptr));
        pushButton_2->setText(QApplication::translate("PlayerUI", "\345\201\234\346\255\242\350\204\232\346\234\254", nullptr));
        label_7->setText(QApplication::translate("PlayerUI", "\346\250\241\346\213\237\345\231\250\351\200\211\346\213\251\357\274\210\346\224\257\346\214\201\351\233\267\347\224\265\357\274\214\346\234\250\346\234\250\357\274\211", nullptr));
        cmb_sim->setItemText(0, QApplication::translate("PlayerUI", "\351\233\267\347\224\265", nullptr));
        cmb_sim->setItemText(1, QApplication::translate("PlayerUI", "MuMu\357\274\210\346\234\250\346\234\250\357\274\211", nullptr));

        btn_startAuto->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213\346\214\202\346\234\272", nullptr));
        groupBox->setTitle(QApplication::translate("PlayerUI", "\351\235\236\346\210\230\346\226\227", nullptr));
        chk_delegate->setText(QApplication::translate("PlayerUI", "\350\207\252\345\212\250\345\247\224\346\211\230", nullptr));
        chk_dev->setText(QApplication::translate("PlayerUI", "\350\207\252\345\212\250\345\274\200\345\217\221", nullptr));
        cmb_dev->setItemText(0, QApplication::translate("PlayerUI", "\351\231\220\346\227\266\345\274\200\345\217\221", nullptr));
        cmb_dev->setItemText(1, QApplication::translate("PlayerUI", "\346\231\256\351\200\232\345\274\200\345\217\22110", nullptr));
        cmb_dev->setItemText(2, QApplication::translate("PlayerUI", "\346\231\256\351\200\232\345\274\200\345\217\22120", nullptr));

        label_8->setText(QApplication::translate("PlayerUI", "\345\210\206\351\222\237\344\270\200\346\254\241", nullptr));
        label_5->setText(QApplication::translate("PlayerUI", "\345\210\206\351\222\237\344\270\200\346\254\241", nullptr));
        cmb_delegate->setItemText(0, QApplication::translate("PlayerUI", "\346\234\200\346\205\242", nullptr));

        edt_delegateInterval->setText(QApplication::translate("PlayerUI", "360", nullptr));
        edt_devInterval->setText(QApplication::translate("PlayerUI", "65", nullptr));
        chk_recruit->setText(QApplication::translate("PlayerUI", "\350\207\252\345\212\250\346\213\233\345\213\237", nullptr));
        label_9->setText(QApplication::translate("PlayerUI", "\345\210\206\351\222\237\344\270\200\346\254\241", nullptr));
        edt_recruitInterval->setText(QApplication::translate("PlayerUI", "240", nullptr));
        btn_noneBattle_toggle->setText(QApplication::translate("PlayerUI", "\345\217\215\351\200\211\351\200\211\351\241\271", nullptr));
        groupBox_2->setTitle(QApplication::translate("PlayerUI", "\346\210\230\346\226\227", nullptr));
        chk_daily->setText(QApplication::translate("PlayerUI", "\346\227\245\345\270\270\346\214\202\346\234\272", nullptr));
        chk_reward->setText(QApplication::translate("PlayerUI", "\350\265\217\351\207\221\346\214\202\346\234\272\357\274\210\346\227\240\351\231\220\346\254\241\346\225\260\357\274\211", nullptr));
        label_6->setText(QApplication::translate("PlayerUI", "\345\210\206\351\222\237\346\243\200\346\237\245\344\270\200\346\254\241", nullptr));
        edt_emergencyInterval->setText(QApplication::translate("PlayerUI", "30", nullptr));
        chk_emergency->setText(QApplication::translate("PlayerUI", "\350\207\252\345\212\250\347\264\247\346\200\245\344\273\273\345\212\241", nullptr));
        btn_battle_toggle->setText(QApplication::translate("PlayerUI", "\345\217\215\351\200\211\351\200\211\351\241\271", nullptr));
        groupBox_3->setTitle(QApplication::translate("PlayerUI", "\346\264\273\345\212\250", nullptr));
        textEdit->setHtml(QApplication::translate("PlayerUI", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\346\264\273\345\212\250\347\232\204\344\274\230\345\205\210\347\272\247\344\270\272\346\234\200\351\253\230\357\274\214\346\211\247\350\241\214\346\264\273\345\212\250\346\227\266\344\270\215\345\206\215\346\211\247\350\241\214\345\205\266\344\273\226\350\204\232\346\234\254\357\274\214\350\213\245\346\264\273\345\212\250\345\267\262\347\273\223\346\235\237\350\257\267\345\213\277\345\206\215\350\277\220\350\241\214\346\264\273\345\212\250\350\204\232\346\234\254</p></body></html>", nullptr));
        btn_huodong1->setText(QApplication::translate("PlayerUI", "\345\274\200\345\247\213t-1-5\350\204\232\346\234\254", nullptr));
        label_10->setText(QApplication::translate("PlayerUI", "\345\234\250\346\216\242\347\264\242\347\225\214\351\235\242\350\277\220\350\241\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerUI: public Ui_PlayerUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYER_H
