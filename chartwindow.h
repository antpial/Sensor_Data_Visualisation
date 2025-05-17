#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>

#include "parsedpacket.h"

QT_CHARTS_USE_NAMESPACE

    namespace Ui {
    class ChartWindow;
}

class ChartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWindow(QWidget *parent = nullptr);
    ~ChartWindow();

public slots:
    void updateFromPacket(const ParsedPacket &packet);


private:
    Ui::ChartWindow *ui;
    QList<QLineSeries*> seriesList;

    void initializeCharts();
};

#endif // CHARTWINDOW_H
