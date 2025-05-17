/********************************************************************************
** Form generated from reading UI file 'chartwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTWINDOW_H
#define UI_CHARTWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartWindow
{
public:
    QGridLayout *gridLayout;
    QtCharts::QChartView *chartView_1;
    QtCharts::QChartView *chartView_2;
    QtCharts::QChartView *chartView_3;
    QtCharts::QChartView *chartView_4;
    QtCharts::QChartView *chartView_5;
    QtCharts::QChartView *chartView_6;
    QtCharts::QChartView *chartView_7;
    QtCharts::QChartView *chartView_8;

    void setupUi(QWidget *ChartWindow)
    {
        if (ChartWindow->objectName().isEmpty())
            ChartWindow->setObjectName(QString::fromUtf8("ChartWindow"));
        ChartWindow->resize(800, 600);
        gridLayout = new QGridLayout(ChartWindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        chartView_1 = new QtCharts::QChartView(ChartWindow);
        chartView_1->setObjectName(QString::fromUtf8("chartView_1"));

        gridLayout->addWidget(chartView_1, 0, 0, 1, 1);

        chartView_2 = new QtCharts::QChartView(ChartWindow);
        chartView_2->setObjectName(QString::fromUtf8("chartView_2"));

        gridLayout->addWidget(chartView_2, 0, 1, 1, 1);

        chartView_3 = new QtCharts::QChartView(ChartWindow);
        chartView_3->setObjectName(QString::fromUtf8("chartView_3"));

        gridLayout->addWidget(chartView_3, 1, 0, 1, 1);

        chartView_4 = new QtCharts::QChartView(ChartWindow);
        chartView_4->setObjectName(QString::fromUtf8("chartView_4"));

        gridLayout->addWidget(chartView_4, 1, 1, 1, 1);

        chartView_5 = new QtCharts::QChartView(ChartWindow);
        chartView_5->setObjectName(QString::fromUtf8("chartView_5"));

        gridLayout->addWidget(chartView_5, 2, 0, 1, 1);

        chartView_6 = new QtCharts::QChartView(ChartWindow);
        chartView_6->setObjectName(QString::fromUtf8("chartView_6"));

        gridLayout->addWidget(chartView_6, 2, 1, 1, 1);

        chartView_7 = new QtCharts::QChartView(ChartWindow);
        chartView_7->setObjectName(QString::fromUtf8("chartView_7"));

        gridLayout->addWidget(chartView_7, 3, 0, 1, 1);

        chartView_8 = new QtCharts::QChartView(ChartWindow);
        chartView_8->setObjectName(QString::fromUtf8("chartView_8"));

        gridLayout->addWidget(chartView_8, 3, 1, 1, 1);


        retranslateUi(ChartWindow);

        QMetaObject::connectSlotsByName(ChartWindow);
    } // setupUi

    void retranslateUi(QWidget *ChartWindow)
    {
        ChartWindow->setWindowTitle(QCoreApplication::translate("ChartWindow", "Chart Window", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChartWindow: public Ui_ChartWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTWINDOW_H
