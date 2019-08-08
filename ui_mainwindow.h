/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *btn_start;
    QPushButton *btn_stop;
    QListWidget *list_tip;
    QLabel *label;
    QLineEdit *edt_wndName;
    QLabel *label_2;
    QLineEdit *edt_wndName2;
    QComboBox *cb_inputType;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *cb_opType;
    QLabel *label_5;
    QLineEdit *edt_vk;
    QPushButton *btn_addInput;
    QPushButton *btn_delLastInput;
    QPushButton *btn_delAllInput;
    QLabel *label_6;
    QLineEdit *edt_delay;
    QLabel *label_7;
    QLineEdit *edt_x;
    QLabel *label_8;
    QLineEdit *edt_y;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *edt_x2;
    QLineEdit *edt_y2;
    QLabel *label_11;
    QLineEdit *edt_repeat;
    QPushButton *btn_save;
    QLineEdit *edt_saveName;
    QPushButton *btn_load;
    QLabel *label_12;
    QListWidget *list_inputVec;
    QLabel *label_13;
    QPushButton *btn_delSelectInput;
    QPushButton *btn_updateSelectInpu;
    QLabel *label_14;
    QLineEdit *edt_picPath;
    QLabel *label_15;
    QLineEdit *edt_rate;
    QLabel *label_16;
    QLineEdit *edt_cmpPic;
    QLabel *label_17;
    QLineEdit *edt_findPicOvertime;
    QLabel *label_18;
    QLineEdit *edt_succeedJump;
    QLabel *label_19;
    QLineEdit *edt_overtimeJump;
    QLabel *label_20;
    QLineEdit *edt_costTime;
    QLabel *label_21;
    QLineEdit *edt_insertIndex;
    QPushButton *btn_insertInput;
    QPushButton *btn_delAllInput_2;
    QLabel *label_22;
    QLineEdit *edt_overtimeJumpModule;
    QLabel *label_23;
    QLineEdit *edt_succeedJumpModule;
    QCheckBox *chk_cmpPicClick;
    QPushButton *btn_openFileDialog;
    QPushButton *btn_openFileDialog_picPath;
    QLabel *label_24;
    QLineEdit *edt_comment;
    QPushButton *btn_updateAllInput;
    QLabel *label_25;
    QLabel *label_26;
    QLineEdit *edt_wndWidth;
    QLineEdit *edt_wndHeight;
    QLineEdit *edt_overwriteNum;
    QLabel *label_27;
    QLineEdit *edt_overwriteIndex;
    QLabel *label_28;
    QPushButton *btn_overwrite;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1039, 655);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        btn_start = new QPushButton(centralWidget);
        btn_start->setObjectName(QString::fromUtf8("btn_start"));
        btn_start->setGeometry(QRect(350, 620, 93, 28));
        btn_stop = new QPushButton(centralWidget);
        btn_stop->setObjectName(QString::fromUtf8("btn_stop"));
        btn_stop->setGeometry(QRect(240, 620, 93, 28));
        list_tip = new QListWidget(centralWidget);
        list_tip->setObjectName(QString::fromUtf8("list_tip"));
        list_tip->setGeometry(QRect(10, 450, 431, 161));
        list_tip->setAutoScroll(true);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 72, 21));
        edt_wndName = new QLineEdit(centralWidget);
        edt_wndName->setObjectName(QString::fromUtf8("edt_wndName"));
        edt_wndName->setGeometry(QRect(90, 10, 211, 21));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 40, 81, 21));
        edt_wndName2 = new QLineEdit(centralWidget);
        edt_wndName2->setObjectName(QString::fromUtf8("edt_wndName2"));
        edt_wndName2->setGeometry(QRect(90, 40, 211, 21));
        cb_inputType = new QComboBox(centralWidget);
        cb_inputType->addItem(QString());
        cb_inputType->addItem(QString());
        cb_inputType->addItem(QString());
        cb_inputType->setObjectName(QString::fromUtf8("cb_inputType"));
        cb_inputType->setGeometry(QRect(10, 140, 91, 22));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 110, 72, 21));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(120, 110, 72, 21));
        cb_opType = new QComboBox(centralWidget);
        cb_opType->addItem(QString());
        cb_opType->addItem(QString());
        cb_opType->addItem(QString());
        cb_opType->addItem(QString());
        cb_opType->setObjectName(QString::fromUtf8("cb_opType"));
        cb_opType->setGeometry(QRect(120, 140, 81, 22));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(240, 110, 81, 21));
        edt_vk = new QLineEdit(centralWidget);
        edt_vk->setObjectName(QString::fromUtf8("edt_vk"));
        edt_vk->setGeometry(QRect(240, 140, 71, 21));
        btn_addInput = new QPushButton(centralWidget);
        btn_addInput->setObjectName(QString::fromUtf8("btn_addInput"));
        btn_addInput->setGeometry(QRect(350, 410, 93, 28));
        btn_delLastInput = new QPushButton(centralWidget);
        btn_delLastInput->setObjectName(QString::fromUtf8("btn_delLastInput"));
        btn_delLastInput->setGeometry(QRect(220, 410, 111, 28));
        btn_delAllInput = new QPushButton(centralWidget);
        btn_delAllInput->setObjectName(QString::fromUtf8("btn_delAllInput"));
        btn_delAllInput->setGeometry(QRect(100, 410, 101, 28));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(340, 110, 91, 21));
        edt_delay = new QLineEdit(centralWidget);
        edt_delay->setObjectName(QString::fromUtf8("edt_delay"));
        edt_delay->setGeometry(QRect(340, 140, 71, 21));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(240, 170, 41, 21));
        edt_x = new QLineEdit(centralWidget);
        edt_x->setObjectName(QString::fromUtf8("edt_x"));
        edt_x->setGeometry(QRect(240, 200, 71, 21));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(340, 170, 41, 21));
        edt_y = new QLineEdit(centralWidget);
        edt_y->setObjectName(QString::fromUtf8("edt_y"));
        edt_y->setGeometry(QRect(340, 200, 71, 21));
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 170, 71, 21));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(120, 170, 71, 21));
        edt_x2 = new QLineEdit(centralWidget);
        edt_x2->setObjectName(QString::fromUtf8("edt_x2"));
        edt_x2->setGeometry(QRect(10, 200, 91, 21));
        edt_y2 = new QLineEdit(centralWidget);
        edt_y2->setObjectName(QString::fromUtf8("edt_y2"));
        edt_y2->setGeometry(QRect(120, 200, 81, 21));
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 230, 71, 21));
        edt_repeat = new QLineEdit(centralWidget);
        edt_repeat->setObjectName(QString::fromUtf8("edt_repeat"));
        edt_repeat->setGeometry(QRect(10, 260, 71, 21));
        btn_save = new QPushButton(centralWidget);
        btn_save->setObjectName(QString::fromUtf8("btn_save"));
        btn_save->setGeometry(QRect(120, 620, 93, 28));
        edt_saveName = new QLineEdit(centralWidget);
        edt_saveName->setObjectName(QString::fromUtf8("edt_saveName"));
        edt_saveName->setGeometry(QRect(110, 320, 261, 21));
        btn_load = new QPushButton(centralWidget);
        btn_load->setObjectName(QString::fromUtf8("btn_load"));
        btn_load->setGeometry(QRect(10, 620, 93, 28));
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(110, 290, 241, 20));
        list_inputVec = new QListWidget(centralWidget);
        list_inputVec->setObjectName(QString::fromUtf8("list_inputVec"));
        list_inputVec->setGeometry(QRect(580, 40, 441, 571));
        list_inputVec->setAutoScroll(true);
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(760, 10, 61, 21));
        btn_delSelectInput = new QPushButton(centralWidget);
        btn_delSelectInput->setObjectName(QString::fromUtf8("btn_delSelectInput"));
        btn_delSelectInput->setGeometry(QRect(820, 620, 101, 28));
        btn_updateSelectInpu = new QPushButton(centralWidget);
        btn_updateSelectInpu->setObjectName(QString::fromUtf8("btn_updateSelectInpu"));
        btn_updateSelectInpu->setGeometry(QRect(580, 620, 101, 28));
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(110, 230, 71, 21));
        edt_picPath = new QLineEdit(centralWidget);
        edt_picPath->setObjectName(QString::fromUtf8("edt_picPath"));
        edt_picPath->setGeometry(QRect(110, 260, 261, 21));
        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(450, 150, 111, 21));
        edt_rate = new QLineEdit(centralWidget);
        edt_rate->setObjectName(QString::fromUtf8("edt_rate"));
        edt_rate->setGeometry(QRect(450, 180, 111, 21));
        label_16 = new QLabel(centralWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(450, 30, 61, 21));
        edt_cmpPic = new QLineEdit(centralWidget);
        edt_cmpPic->setObjectName(QString::fromUtf8("edt_cmpPic"));
        edt_cmpPic->setGeometry(QRect(450, 60, 61, 21));
        label_17 = new QLabel(centralWidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(10, 290, 61, 21));
        edt_findPicOvertime = new QLineEdit(centralWidget);
        edt_findPicOvertime->setObjectName(QString::fromUtf8("edt_findPicOvertime"));
        edt_findPicOvertime->setGeometry(QRect(10, 320, 71, 21));
        label_18 = new QLabel(centralWidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(450, 210, 111, 21));
        edt_succeedJump = new QLineEdit(centralWidget);
        edt_succeedJump->setObjectName(QString::fromUtf8("edt_succeedJump"));
        edt_succeedJump->setGeometry(QRect(450, 240, 111, 21));
        label_19 = new QLabel(centralWidget);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(450, 270, 111, 21));
        edt_overtimeJump = new QLineEdit(centralWidget);
        edt_overtimeJump->setObjectName(QString::fromUtf8("edt_overtimeJump"));
        edt_overtimeJump->setGeometry(QRect(450, 300, 111, 21));
        label_20 = new QLabel(centralWidget);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(450, 90, 111, 21));
        edt_costTime = new QLineEdit(centralWidget);
        edt_costTime->setObjectName(QString::fromUtf8("edt_costTime"));
        edt_costTime->setGeometry(QRect(450, 120, 111, 21));
        label_21 = new QLabel(centralWidget);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setGeometry(QRect(450, 490, 31, 31));
        edt_insertIndex = new QLineEdit(centralWidget);
        edt_insertIndex->setObjectName(QString::fromUtf8("edt_insertIndex"));
        edt_insertIndex->setGeometry(QRect(490, 490, 61, 31));
        btn_insertInput = new QPushButton(centralWidget);
        btn_insertInput->setObjectName(QString::fromUtf8("btn_insertInput"));
        btn_insertInput->setGeometry(QRect(450, 530, 71, 28));
        btn_delAllInput_2 = new QPushButton(centralWidget);
        btn_delAllInput_2->setObjectName(QString::fromUtf8("btn_delAllInput_2"));
        btn_delAllInput_2->setGeometry(QRect(10, 410, 71, 28));
        label_22 = new QLabel(centralWidget);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setGeometry(QRect(450, 330, 111, 21));
        edt_overtimeJumpModule = new QLineEdit(centralWidget);
        edt_overtimeJumpModule->setObjectName(QString::fromUtf8("edt_overtimeJumpModule"));
        edt_overtimeJumpModule->setGeometry(QRect(450, 420, 111, 21));
        label_23 = new QLabel(centralWidget);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setGeometry(QRect(450, 390, 111, 21));
        edt_succeedJumpModule = new QLineEdit(centralWidget);
        edt_succeedJumpModule->setObjectName(QString::fromUtf8("edt_succeedJumpModule"));
        edt_succeedJumpModule->setGeometry(QRect(450, 360, 111, 21));
        chk_cmpPicClick = new QCheckBox(centralWidget);
        chk_cmpPicClick->setObjectName(QString::fromUtf8("chk_cmpPicClick"));
        chk_cmpPicClick->setGeometry(QRect(450, 460, 111, 19));
        chk_cmpPicClick->setChecked(true);
        btn_openFileDialog = new QPushButton(centralWidget);
        btn_openFileDialog->setObjectName(QString::fromUtf8("btn_openFileDialog"));
        btn_openFileDialog->setGeometry(QRect(380, 320, 31, 21));
        btn_openFileDialog_picPath = new QPushButton(centralWidget);
        btn_openFileDialog_picPath->setObjectName(QString::fromUtf8("btn_openFileDialog_picPath"));
        btn_openFileDialog_picPath->setGeometry(QRect(380, 260, 31, 21));
        label_24 = new QLabel(centralWidget);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setGeometry(QRect(20, 70, 61, 21));
        edt_comment = new QLineEdit(centralWidget);
        edt_comment->setObjectName(QString::fromUtf8("edt_comment"));
        edt_comment->setGeometry(QRect(90, 70, 331, 21));
        btn_updateAllInput = new QPushButton(centralWidget);
        btn_updateAllInput->setObjectName(QString::fromUtf8("btn_updateAllInput"));
        btn_updateAllInput->setGeometry(QRect(700, 620, 101, 28));
        label_25 = new QLabel(centralWidget);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setGeometry(QRect(310, 10, 51, 21));
        label_26 = new QLabel(centralWidget);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setGeometry(QRect(310, 40, 51, 21));
        edt_wndWidth = new QLineEdit(centralWidget);
        edt_wndWidth->setObjectName(QString::fromUtf8("edt_wndWidth"));
        edt_wndWidth->setGeometry(QRect(360, 10, 61, 21));
        edt_wndHeight = new QLineEdit(centralWidget);
        edt_wndHeight->setObjectName(QString::fromUtf8("edt_wndHeight"));
        edt_wndHeight->setGeometry(QRect(360, 40, 61, 21));
        edt_overwriteNum = new QLineEdit(centralWidget);
        edt_overwriteNum->setObjectName(QString::fromUtf8("edt_overwriteNum"));
        edt_overwriteNum->setGeometry(QRect(140, 380, 61, 21));
        label_27 = new QLabel(centralWidget);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        label_27->setGeometry(QRect(140, 350, 91, 21));
        edt_overwriteIndex = new QLineEdit(centralWidget);
        edt_overwriteIndex->setObjectName(QString::fromUtf8("edt_overwriteIndex"));
        edt_overwriteIndex->setGeometry(QRect(240, 380, 91, 21));
        label_28 = new QLabel(centralWidget);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        label_28->setGeometry(QRect(240, 350, 91, 21));
        btn_overwrite = new QPushButton(centralWidget);
        btn_overwrite->setObjectName(QString::fromUtf8("btn_overwrite"));
        btn_overwrite->setGeometry(QRect(340, 360, 71, 31));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);
        QObject::connect(btn_start, SIGNAL(clicked()), MainWindow, SLOT(OnBtnStartClick()));
        QObject::connect(btn_stop, SIGNAL(clicked()), MainWindow, SLOT(OnBtnStopClick()));
        QObject::connect(btn_addInput, SIGNAL(clicked()), MainWindow, SLOT(OnBtnAddInput()));
        QObject::connect(btn_delLastInput, SIGNAL(clicked()), MainWindow, SLOT(OnBtnDelLastInput()));
        QObject::connect(btn_delAllInput, SIGNAL(clicked()), MainWindow, SLOT(OnBtnDelAllInput()));
        QObject::connect(btn_save, SIGNAL(clicked()), MainWindow, SLOT(OnBtnSaveClick()));
        QObject::connect(btn_load, SIGNAL(clicked()), MainWindow, SLOT(OnBtnLoadClick()));
        QObject::connect(btn_delSelectInput, SIGNAL(clicked()), MainWindow, SLOT(OnBtnDelSelectInputClick()));
        QObject::connect(list_inputVec, SIGNAL(clicked(QModelIndex)), MainWindow, SLOT(OnBtnInputListClick()));
        QObject::connect(btn_updateSelectInpu, SIGNAL(clicked()), MainWindow, SLOT(OnBtnUpdateSelectInputClick()));
        QObject::connect(btn_insertInput, SIGNAL(clicked()), MainWindow, SLOT(OnBtnInsertInputClick()));
        QObject::connect(btn_delAllInput_2, SIGNAL(clicked()), MainWindow, SLOT(OnBtnClearTipInfo()));
        QObject::connect(list_inputVec, SIGNAL(itemSelectionChanged()), MainWindow, SLOT(OnBtnInputListClick()));
        QObject::connect(btn_openFileDialog, SIGNAL(clicked()), MainWindow, SLOT(OnBtnOpenFileDialog()));
        QObject::connect(btn_openFileDialog_picPath, SIGNAL(clicked()), MainWindow, SLOT(OnBtnOpenFileDialog_PicPath()));
        QObject::connect(btn_updateAllInput, SIGNAL(clicked()), MainWindow, SLOT(OnBtnUpdateAllInput()));
        QObject::connect(btn_overwrite, SIGNAL(clicked()), MainWindow, SLOT(OnBtnOverwrite()));
        QObject::connect(list_inputVec, SIGNAL(doubleClicked(QModelIndex)), MainWindow, SLOT(OnBtnSetOverwriteTargetIndex()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        btn_start->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\345\217\221\351\200\201", nullptr));
        btn_stop->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\345\217\221\351\200\201", nullptr));
        label->setText(QApplication::translate("MainWindow", "\347\252\227\345\217\243\345\220\215\347\247\260", nullptr));
        edt_wndName->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "\345\255\220\347\252\227\345\217\243\345\220\215\347\247\260", nullptr));
        edt_wndName2->setText(QString());
        cb_inputType->setItemText(0, QApplication::translate("MainWindow", "\351\274\240\346\240\207\350\276\223\345\205\245", nullptr));
        cb_inputType->setItemText(1, QApplication::translate("MainWindow", "\351\224\256\347\233\230\350\276\223\345\205\245", nullptr));
        cb_inputType->setItemText(2, QApplication::translate("MainWindow", "\345\233\276\347\211\207\350\257\206\345\210\253", nullptr));

        label_3->setText(QApplication::translate("MainWindow", "\350\276\223\345\205\245\347\261\273\345\236\213", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "\346\223\215\344\275\234\347\261\273\345\236\213", nullptr));
        cb_opType->setItemText(0, QApplication::translate("MainWindow", "\347\202\271\345\207\273", nullptr));
        cb_opType->setItemText(1, QApplication::translate("MainWindow", "\346\214\211\344\275\217", nullptr));
        cb_opType->setItemText(2, QApplication::translate("MainWindow", "\347\247\273\345\212\250", nullptr));
        cb_opType->setItemText(3, QApplication::translate("MainWindow", "\351\207\212\346\224\276", nullptr));

        label_5->setText(QApplication::translate("MainWindow", "\350\231\232\346\213\237\346\214\211\351\224\256\345\200\274", nullptr));
        edt_vk->setText(QApplication::translate("MainWindow", "G", nullptr));
        btn_addInput->setText(QApplication::translate("MainWindow", "\346\267\273\345\212\240\350\276\223\345\205\245", nullptr));
        btn_delLastInput->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\344\270\212\344\270\200\344\270\252\350\276\223\345\205\245", nullptr));
        btn_delAllInput->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\346\211\200\346\234\211\350\276\223\345\205\245", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "\345\273\266\350\277\237\357\274\210\346\257\253\347\247\222\357\274\211", nullptr));
        edt_delay->setText(QApplication::translate("MainWindow", "100", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "\351\274\240\346\240\207X", nullptr));
        edt_x->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "\351\274\240\346\240\207Y", nullptr));
        edt_y->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "\351\274\240\346\240\207X_2", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "\351\274\240\346\240\207Y_2", nullptr));
        edt_x2->setText(QApplication::translate("MainWindow", "1", nullptr));
        edt_y2->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "\351\207\215\345\244\215\346\254\241\346\225\260", nullptr));
        edt_repeat->setText(QApplication::translate("MainWindow", "1", nullptr));
        btn_save->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\351\205\215\347\275\256", nullptr));
        edt_saveName->setText(QString());
        btn_load->setText(QApplication::translate("MainWindow", "\345\212\240\350\275\275\351\205\215\347\275\256", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "\346\226\207\344\273\266\344\275\215\347\275\256\357\274\232\351\273\230\350\256\244\344\277\235\345\255\230\345\234\250D:/QtSave\344\270\213", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "\345\221\275\344\273\244\345\210\227\350\241\250", nullptr));
        btn_delSelectInput->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\351\200\211\344\270\255\345\221\275\344\273\244", nullptr));
        btn_updateSelectInpu->setText(QApplication::translate("MainWindow", "\346\233\264\346\226\260\351\200\211\344\270\255\345\221\275\344\273\244", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "\345\233\276\347\211\207\350\267\257\345\276\204", nullptr));
        edt_picPath->setText(QString());
        label_15->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\345\257\271\346\257\224\347\233\270\344\274\274\345\272\246", nullptr));
        edt_rate->setText(QString());
        label_16->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\346\214\207\344\273\244", nullptr));
        edt_cmpPic->setText(QString());
        label_17->setText(QApplication::translate("MainWindow", "\345\233\276\347\211\207\350\266\205\346\227\266", nullptr));
        edt_findPicOvertime->setText(QApplication::translate("MainWindow", "-1", nullptr));
        label_18->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224\346\210\220\345\212\237\350\267\263\350\275\254", nullptr));
        edt_succeedJump->setText(QApplication::translate("MainWindow", "-1", nullptr));
        label_19->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224\350\266\205\346\227\266\350\267\263\350\275\254", nullptr));
        edt_overtimeJump->setText(QApplication::translate("MainWindow", "-1", nullptr));
        label_20->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224\350\200\227\346\227\266(\347\274\251\346\224\276)", nullptr));
        edt_costTime->setText(QString());
        label_21->setText(QApplication::translate("MainWindow", "\347\264\242\345\274\225", nullptr));
        edt_insertIndex->setText(QString());
        btn_insertInput->setText(QApplication::translate("MainWindow", "\346\217\222\345\205\245\350\276\223\345\205\245", nullptr));
        btn_delAllInput_2->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272\346\217\220\347\244\272", nullptr));
        label_22->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224\346\210\220\345\212\237\346\250\241\345\235\227", nullptr));
#ifndef QT_NO_TOOLTIP
        edt_overtimeJumpModule->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        edt_overtimeJumpModule->setText(QApplication::translate("MainWindow", "./QtSave/", nullptr));
        label_23->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224\350\266\205\346\227\266\346\250\241\345\235\227", nullptr));
#ifndef QT_NO_TOOLTIP
        edt_succeedJumpModule->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        edt_succeedJumpModule->setText(QApplication::translate("MainWindow", "./QtSave/", nullptr));
        chk_cmpPicClick->setText(QApplication::translate("MainWindow", "\345\257\271\346\257\224\345\220\216\347\202\271\345\207\273", nullptr));
        btn_openFileDialog->setText(QApplication::translate("MainWindow", "...", nullptr));
        btn_openFileDialog_picPath->setText(QApplication::translate("MainWindow", "...", nullptr));
        label_24->setText(QApplication::translate("MainWindow", "\346\223\215\344\275\234\346\263\250\351\207\212", nullptr));
        edt_comment->setText(QString());
        btn_updateAllInput->setText(QApplication::translate("MainWindow", "\346\233\264\346\226\260\346\211\200\346\234\211\345\221\275\344\273\244", nullptr));
        label_25->setText(QApplication::translate("MainWindow", "\347\252\227\345\217\243\345\256\275", nullptr));
        label_26->setText(QApplication::translate("MainWindow", "\347\252\227\345\217\243\351\253\230", nullptr));
        edt_wndWidth->setText(QApplication::translate("MainWindow", "890", nullptr));
        edt_wndHeight->setText(QApplication::translate("MainWindow", "588", nullptr));
        edt_overwriteNum->setText(QApplication::translate("MainWindow", "2", nullptr));
        label_27->setText(QApplication::translate("MainWindow", "\350\246\206\347\233\226\347\233\256\346\240\207\346\225\260\351\207\217", nullptr));
        edt_overwriteIndex->setText(QString());
        label_28->setText(QApplication::translate("MainWindow", "\350\246\206\347\233\226\347\233\256\346\240\207\347\264\242\345\274\225", nullptr));
        btn_overwrite->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\350\246\206\347\233\226", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
