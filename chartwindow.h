#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>

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

private slots:
    void updateCharts();

private:
    Ui::ChartWindow *ui;
    QList<QLineSeries*> seriesList;
    QTimer *timer;

    void initializeCharts();
};

#endif // CHARTWINDOW_H
