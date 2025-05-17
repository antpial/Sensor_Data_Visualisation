#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chartwindow.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleNewPacket(const ParsedPacket &packet)
{
    QString line;
    for (int i = 0; i < packet.sensors.size(); ++i) {
        line += QString("Sensor %1: %2    ").arg(i).arg(packet.sensors[i], 0, 'f', 2);
    }
    ui->sensorData->setText(line.trimmed());

    if (!packet.log.isEmpty()) {
        ui->console->append(packet.log);
    }

    // TODO: opcjonalnie przekazać dane do ChartWindow, np. emitując własny sygnał
}

void MainWindow::handleSerialError(const QString &error)
{
    ui->console->append("Błąd: " + error);
}
