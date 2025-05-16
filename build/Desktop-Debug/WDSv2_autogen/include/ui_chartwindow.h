/********************************************************************************
** Form generated from reading UI file 'chartwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTWINDOW_H
#define UI_CHARTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartWindow
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *ChartWindow)
    {
        if (ChartWindow->objectName().isEmpty())
            ChartWindow->setObjectName(QString::fromUtf8("ChartWindow"));
        ChartWindow->resize(400, 300);
        pushButton = new QPushButton(ChartWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(150, 130, 88, 26));

        retranslateUi(ChartWindow);

        QMetaObject::connectSlotsByName(ChartWindow);
    } // setupUi

    void retranslateUi(QWidget *ChartWindow)
    {
        ChartWindow->setWindowTitle(QCoreApplication::translate("ChartWindow", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("ChartWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChartWindow: public Ui_ChartWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTWINDOW_H
