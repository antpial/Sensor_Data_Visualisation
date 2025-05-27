#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chartwindow.h"
#include "mapwindow.h"


#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QQmlContext>
#include <QVBoxLayout>
#include <QQuickItem>

#define PORT_PATH "/dev/pts/2"
#define CRITICAL_DEPTH 50


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Project FOKA ground base");
    this->setStyleSheet("background-color: #ffffff;"); // jasnoszary kolor
    ui->console->setStyleSheet("background-color: #ffffff;");
    ui->sensorBar->setStyleSheet("background-color: #ffffff");

    // Serial manager
    serialManager = new SerialManager(this);
    connect(serialManager, &SerialManager::newPacketReceived, this, &MainWindow::handleNewPacket);
    connect(serialManager, &SerialManager::serialError, this, &MainWindow::handleSerialError);
    serialManager->start(PORT_PATH);

    // Wykresy
    chartWindow = new ChartWindow();
    chartWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(serialManager, &SerialManager::newPacketReceived, chartWindow, &ChartWindow::updateFromPacket);
    chartWindow->show();

    // Mapa
    // QGraphicsScene *scene = new QGraphicsScene(this);
    // QPixmap pixmap("/home/antek/Downloads/map.jpg");
    // scene->addItem(new QGraphicsPixmapItem(pixmap));
    // ui->map->setScene(scene);
    // ui->sensorBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // ui->sensorBar->setMinimumHeight(100);

    ui->map->setSource(QUrl::fromLocalFile("/home/antek/WDSv2/mapa.qml"));
    ui->map->setResizeMode(QQuickWidget::SizeRootObjectToView);


    // mapWindow = new MapWindow();
    // mapWindow->setAttribute(Qt::WA_DeleteOnClose);
    // mapWindow->show();


    //tabliczki z sensorami
    sensorConfig = loadSensorConfig("charts_config.json"); // ścieżka do pliku JSON



}



MainWindow::~MainWindow()
{
    delete ui;
}


QVector<QPair<QString, QString>> MainWindow::loadSensorConfig(const QString &filePath)
{
    QVector<QPair<QString, QString>> sensors; // <nazwa, jednostka>

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Nie można otworzyć pliku:" << filePath;
        return sensors;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Niepoprawny JSON";
        return sensors;
    }

    QJsonObject rootObj = doc.object();
    QJsonArray charts = rootObj["charts"].toArray();

    for (const QJsonValue &val : charts) {
        QJsonObject obj = val.toObject();
        QString name = obj["sensor"].toString();
        QString unit = obj["unit_y"].toString();
        sensors.append(qMakePair(name, unit));
    }

    return sensors;
}



void MainWindow::handleNewPacket(const ParsedPacket &packet)
{
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->sensorBar->layout());
    if (!layout) return;

    if (sensorTiles.size() != packet.sensors.size()) {
        // Czyszczenie starych kafelków
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        sensorTiles.clear();

        for (int i = 0; i < packet.sensors.size(); ++i) {
            QFrame* frame = new QFrame(this);
            frame->setFrameShape(QFrame::StyledPanel);
            frame->setFrameShadow(QFrame::Raised);
            // frame->setStyleSheet("QFrame { background-color: #c9c7c7; border-radius: 8px; padding: 1px; }");
            frame->setMaximumWidth(150);
            frame->setMinimumHeight(60);
            frame->setMaximumHeight(120);
            frame->setLineWidth(1);                   // grubość linii
            frame->setMidLineWidth(0);
            frame->setStyleSheet(
                "QFrame {"
                "  border: 1px solid #000000;"   // kolor i grubość obramowania
                "  border-radius: 6px;"          // zaokrąglenie rogów
                "  background-color: #ffffff;"   // kolor tła wewnątrz ramki
                "}"
                );




            QVBoxLayout* vbox = new QVBoxLayout(frame);

            QString name = (i < sensorConfig.size()) ? sensorConfig[i].first : QString("Sensor %1").arg(i);
            QString unit = (i < sensorConfig.size()) ? sensorConfig[i].second : "";

            QLabel* nameLabel = new QLabel(name, frame);
            nameLabel->setAlignment(Qt::AlignCenter);
            nameLabel->setStyleSheet("font-size: 20px;border: 0px solid #000000;");

            QLabel* valueLabel = new QLabel("0.00 " + unit, frame);
            valueLabel->setAlignment(Qt::AlignCenter);
            valueLabel->setStyleSheet("font-size: 20px;border: 0px solid #000000;");

            vbox->addWidget(nameLabel);
            vbox->addWidget(valueLabel);

            layout->addWidget(frame);

            sensorTiles.append({frame, nameLabel, valueLabel});
        }
    }

    for (int i = 0; i < packet.sensors.size(); ++i) {
        QString unit = (i < sensorConfig.size()) ? sensorConfig[i].second : "";
        sensorTiles[i].valueLabel->setText(QString::number(packet.sensors[i], 'f', 2) + " " + unit);
        if(sensorTiles[i].nameLabel->text() == "Depth"){
            if(packet.sensors[i] < CRITICAL_DEPTH){
                sensorTiles[i].frame->setStyleSheet("QFrame { background-color: #f52f2f; border-radius: 6px; padding: 1px;border: 1px solid #000000;}");
            }else{
                sensorTiles[i].frame->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 6px; padding: 1px;border: 1px solid #000000;}");
            }
        }
    }

    if (!packet.log.isEmpty()) {
        ui->console->append(packet.log);
    }

    //mapa
    if (packet.hasPosition) {
        updatePosition(packet.latitude, packet.longitude);
    }

}


void MainWindow::updatePosition(double latitude, double longitude)
{
    QObject *rootObject = ui->map->rootObject();
    if (!rootObject) {
        qWarning() << "Root object is null!";
        return;
    }

    QVariant lat = latitude;
    QVariant lon = longitude;

    bool invoked = QMetaObject::invokeMethod(rootObject, "updateBoatPosition",
                                             Q_ARG(QVariant, lat),
                                             Q_ARG(QVariant, lon));
    if (!invoked) {
        qWarning() << "Failed to invoke updateBoatPosition!";
    }
}




void MainWindow::handleSerialError(const QString &error)
{
    ui->console->append("Błąd: " + error);
}
