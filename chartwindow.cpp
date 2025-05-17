#include "chartwindow.h"
#include "ui_chartwindow.h"
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QRandomGenerator>
#include <QDebug>

#define NUM_OF_SAMPLES 20

struct ChartConfig {
    QString sensorName;
    double yRange;
    QString unitY;
    QColor color;
};

QVector<ChartConfig> loadChartConfigs(const QString &filePath)
{
    QVector<ChartConfig> configs;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Nie można otworzyć pliku:" << filePath;
        return configs;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning() << "Nieprawidłowy format JSON";
        return configs;
    }

    QJsonArray chartsArray = doc.object()["charts"].toArray();
    for (const QJsonValue &value : chartsArray) {
        QJsonObject obj = value.toObject();
        ChartConfig config;
        config.sensorName = obj["sensor"].toString();
        config.yRange = obj["y_range"].toDouble();
        config.unitY = obj["unit_y"].toString();

        QString colorStr = obj["color"].toString("#000000"); // domyślnie czarny
        config.color = QColor(colorStr);

        configs.append(config);
    }

    return configs;
}


ChartWindow::ChartWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
    initializeCharts();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ChartWindow::updateCharts);
    timer->start(1000); // aktualizacja co 1 sekunda
}

ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::initializeCharts()
{

    QList<QChartView*> views = {
        ui->chartView_1, ui->chartView_2, ui->chartView_3, ui->chartView_4,
        ui->chartView_5, ui->chartView_6, ui->chartView_7, ui->chartView_8
    };

    QVector<ChartConfig> chartConfigs = loadChartConfigs("charts_config.json");

    for (int i = 0; i < views.size(); ++i) {
        QLineSeries *series = new QLineSeries();
        QChart *chart = new QChart();
        // chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->addSeries(series);

        QString sensorName = QString("Sensor %1").arg(i + 1);
        double yRange = 100.0;
        QString unitY = "";
        QColor color;

        if (i < chartConfigs.size()) {
            sensorName = chartConfigs[i].sensorName;
            yRange = chartConfigs[i].yRange;
            unitY = chartConfigs[i].unitY;
            color = chartConfigs[i].color;
        }

        QValueAxis *axisX = new QValueAxis();
        axisX->setRange(0, 20);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, yRange);
        axisY->setTitleText(unitY);

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);

        chart->setTitle(sensorName);
        views[i]->setChart(chart);
        views[i]->setRenderHint(QPainter::Antialiasing);

        QPen pen(color);
        pen.setWidth(2); // grubość linii
        series->setPen(pen);

        seriesList.append(series);

        for (int j = 0; j < 10; ++j) {
            series->append(j, QRandomGenerator::global()->bounded(yRange));
        }
    }

}

void ChartWindow::updateCharts()
{
    for (QLineSeries *series : seriesList) {
        QVector<QPointF> points = series->pointsVector();
        if (points.size() >= NUM_OF_SAMPLES)
            points.removeFirst();  // przesuwamy wykres

        qreal x = points.isEmpty() ? 0 : points.last().x() + 1;
        qreal y = QRandomGenerator::global()->bounded(100);
        points.append(QPointF(x, y));

        series->replace(points);

        QChart *chart = series->chart();
        if (chart && chart->axisX()) {
            qreal xMax = points.last().x();
            qreal xMin = xMax - NUM_OF_SAMPLES;  // Zakładamy, że pokazujemy ostatnie 20 punktów
            if (xMin < 0) xMin = 0;

            QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axisX());
            if (axisX)
                axisX->setRange(xMin, xMax);
        }

    }
}


