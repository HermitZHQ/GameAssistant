/********************************************************************************
** Form generated from reading UI file 'bkg.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BKG_H
#define UI_BKG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BkgUI
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *BkgUI)
    {
        if (BkgUI->objectName().isEmpty())
            BkgUI->setObjectName(QString::fromUtf8("BkgUI"));
        BkgUI->resize(652, 413);
        centralwidget = new QWidget(BkgUI);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        BkgUI->setCentralWidget(centralwidget);
        menubar = new QMenuBar(BkgUI);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 652, 26));
        BkgUI->setMenuBar(menubar);
        statusbar = new QStatusBar(BkgUI);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        BkgUI->setStatusBar(statusbar);

        retranslateUi(BkgUI);

        QMetaObject::connectSlotsByName(BkgUI);
    } // setupUi

    void retranslateUi(QMainWindow *BkgUI)
    {
        BkgUI->setWindowTitle(QApplication::translate("BkgUI", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BkgUI: public Ui_BkgUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BKG_H
