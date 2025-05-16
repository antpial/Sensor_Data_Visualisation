/**
 * @file mainwindow.cpp
 * @brief Implementacja klasy MainWindow odpowiedzialnej za główne okno aplikacji Project FOKA.
 */

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chartwindow.h"

#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <cmath>
#include <QDateTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

/**
 * @struct ParsedPacket
 * @brief Struktura reprezentująca paczkę danych odebraną z portu szeregowego.
 *
 * Zawiera dane z czujników oraz opcjonalny log tekstowy.
 */
struct ParsedPacket {
    QVector<double> sensors; ///< Wartości z czujników.
    QString log;             ///< Wiadomość logu (jeśli występuje).
};

/**
 * @brief Parsuje linię tekstową z portu szeregowego do struktury ParsedPacket.
 * @param line Linia tekstowa do sparsowania.
 * @return Sparsowana paczka danych.
 */
ParsedPacket parseSerialLine(const QString& line)
{
    ParsedPacket result;

    QString cleanLine = line.trimmed();
    int logIndex = cleanLine.indexOf('!');

    QString dataPart = (logIndex >= 0) ? cleanLine.left(logIndex) : cleanLine;
    QString logPart = (logIndex >= 0) ? cleanLine.mid(logIndex + 1) : "";

    QStringList items = dataPart.split(';', Qt::SkipEmptyParts);
    for (const QString& item : items) {
        bool ok;
        double val = item.toDouble(&ok);
        if (ok) result.sensors.append(val);
    }

    result.log = logPart;
    return result;
}

/**
 * @brief Konstruktor klasy MainWindow.
 * @param parent Wskaźnik na rodzica (opcjonalnie).
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Project FOKA ground base");

    logTimer = new QTimer(this);
    QTimer::singleShot(0, this, &MainWindow::setupSerial);

    // Timer do okresowej aktualizacji (obecnie nieużywany)
    // connect(logTimer, &QTimer::timeout, this, &MainWindow::logStep);
    // connect(logTimer, &QTimer::timeout, this, &MainWindow::updateSensorData);
    logTimer->start(1000);

    // Uruchamiam okno wykresow
    ChartWindow *chartWindow = new ChartWindow(); // bez "this"
    chartWindow->setAttribute(Qt::WA_DeleteOnClose); // okno samo się usuwa po zamknięciu
    chartWindow->show(); // pokazujemy niezależne okno

    QGraphicsScene *scene = new QGraphicsScene(this);
    QPixmap pixmap("/home/antek/Downloads/map.jpg");
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    scene->addItem(item);
    ui->map->setScene(scene);
}

/**
 * @brief Destruktor klasy MainWindow.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Ustawia i otwiera połączenie szeregowe.
 */
void MainWindow::setupSerial()
{
    serial = new QSerialPort(this);
    serial->setPortName("/dev/pts/2");  // Upewnij się, że ten port jest poprawny
    serial->setBaudRate(QSerialPort::Baud9600);

    if (serial->open(QIODevice::ReadOnly)) {
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
        ui->console->append("Port otwarty.");
    } else {
        ui->console->append("Nie można otworzyć portu.");
    }
}

/**
 * @brief Odczytuje i przetwarza dane przychodzące z portu szeregowego.
 */
void MainWindow::readData()
{
    QString sensorLine;

    while (serial->canReadLine()) {
        QByteArray data = serial->readLine();
        QString line = QString::fromUtf8(data).trimmed();
        ParsedPacket packet = parseSerialLine(line);

        qDebug() << "Sensors:";
        for (int i = 0; i < packet.sensors.size(); ++i) {
            double value = packet.sensors[i];
            sensorLine += QString("Sensor %1: %2    ").arg(i).arg(value, 0, 'f', 2);
        }

        ui->sensorData->setText(sensorLine.trimmed() + "\n");

        if (!packet.log.isEmpty()) {
            qDebug() << "Log:" << packet.log;
            ui->console->append(packet.log);
        }
    }
}

/**
 * @brief Dodaje nowy wpis do logu tekstowego w interfejsie.
 */
void MainWindow::logStep()
{
    logCounter++;
    QString logText = QString("Wiersz logu #%1").arg(logCounter);
    ui->console->append(logText);
}

/**
 * @brief Aktualizuje dane z czujników symulowanych i wyświetla je w interfejsie.
 */
void MainWindow::updateSensorData()
{
    const int sensorCount = 8;
    const double pi = 3.14159265;
    QString line1, line2;

    for (int i = 0; i < sensorCount; ++i) {
        qint64 seconds = QDateTime::currentSecsSinceEpoch();
        double phaseShift = (2 * pi / sensorCount) * i;
        double value = std::sin(seconds + phaseShift);
        QString text = QString("sensor%1: %2").arg(i + 1).arg(value, 0, 'f', 3);

        if (i < 4)
            line1 += text + "\t";
        else
            line2 += text + "\t";
    }

    ui->sensorData->setText(line1.trimmed() + "\n" + line2.trimmed());
}
