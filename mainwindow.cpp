#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chartwindow.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Project FOKA ground base");

    // Serial manager
    serialManager = new SerialManager(this);
    connect(serialManager, &SerialManager::newPacketReceived, this, &MainWindow::handleNewPacket);
    connect(serialManager, &SerialManager::serialError, this, &MainWindow::handleSerialError);
    serialManager->start("/dev/pts/2");

    // Wykresy
    chartWindow = new ChartWindow();
    chartWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(serialManager, &SerialManager::newPacketReceived, chartWindow, &ChartWindow::updateFromPacket);
    chartWindow->show();

    // Mapa
    QGraphicsScene *scene = new QGraphicsScene(this);
    QPixmap pixmap("/home/antek/Downloads/map.jpg");
    scene->addItem(new QGraphicsPixmapItem(pixmap));
    ui->map->setScene(scene);
    ui->sensorBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->sensorBar->setMinimumHeight(100);

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
            frame->setStyleSheet("QFrame { background-color: #c9c7c7; border-radius: 8px; padding: 5px; }");
            frame->setMaximumWidth(150); // lub inna liczba, np. 150


            QVBoxLayout* vbox = new QVBoxLayout(frame);

            QString name = (i < sensorConfig.size()) ? sensorConfig[i].first : QString("Sensor %1").arg(i);
            QString unit = (i < sensorConfig.size()) ? sensorConfig[i].second : "";

            QLabel* nameLabel = new QLabel(name, frame);
            nameLabel->setAlignment(Qt::AlignCenter);
            nameLabel->setStyleSheet("font-weight: bold;");

            QLabel* valueLabel = new QLabel("0.00 " + unit, frame);
            valueLabel->setAlignment(Qt::AlignCenter);
            valueLabel->setStyleSheet("font-size: 16px;");

            vbox->addWidget(nameLabel);
            vbox->addWidget(valueLabel);

            layout->addWidget(frame);

            sensorTiles.append({frame, nameLabel, valueLabel});
        }
    }

    for (int i = 0; i < packet.sensors.size(); ++i) {
        QString unit = (i < sensorConfig.size()) ? sensorConfig[i].second : "";
        sensorTiles[i].valueLabel->setText(QString::number(packet.sensors[i], 'f', 2) + " " + unit);
    }

    if (!packet.log.isEmpty()) {
        ui->console->append(packet.log);
    }
}





void MainWindow::handleSerialError(const QString &error)
{
    ui->console->append("Błąd: " + error);
}
