/**
 * @file mainwindow.cpp
 * @brief Implementacja klasy MainWindow.
 */


#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <cmath>
#include <QDateTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>


struct ParsedPacket {
    QVector<double> sensors;
    QString log;
};

ParsedPacket parseSerialLine(const QString& line);


/**
 * @brief Aktualizuje dane z czujników i wyświetla je w oknie.
 *
 * Funkcja symuluje wartości czujników sinusoidalnie zmieniające się w czasie.
 * Wyniki są wyświetlane w QTextBrowser w dwóch liniach po 4 czujniki.
 */
void MainWindow::updateSensorData()
{
    const int sensorCount = 8;
    const double pi = 3.14159265;

    QStringList lines;
    QString line1, line2;

    for (int i = 0; i < sensorCount; ++i) {
        qint64 seconds = QDateTime::currentSecsSinceEpoch();
        double phaseShift = (2 * pi / sensorCount) * i;
        double value = std::sin(seconds + phaseShift);

        QString text = QString("sensor%1: %2").arg(i + 1).arg(value, 0, 'f', 3); // 3 miejsca po przecinku

        if (i < 4)
            line1 += text + "\t";
        else
            line2 += text + "\t";
    }

    // Zapisz tekst do QTextBrowser
    ui->sensorData->setText(line1.trimmed() + "\n" + line2.trimmed());
}

/**
 * @brief Dodaje nowy wpis do logu tekstowego.
 *
 * Funkcja zwiększa licznik i dodaje nowy wiersz do QTextBrowser reprezentującego konsolę logu.
 */
void MainWindow::logStep()
{
    logCounter++;
    QString logText = QString("Wiersz logu #%1").arg(logCounter);

    // Dodajemy tekst do QTextBrowser, zachowując poprzedni
    ui->console->append(logText);
}


void MainWindow::readData() {
    QString sensorLine;
    while (serial->canReadLine()) {
        QByteArray data = serial->readLine();
        // ui->console->append("Odebrano: " + QString::fromUtf8(data).trimmed());
        QString line = QString::fromUtf8(data).trimmed();

        ParsedPacket packet = parseSerialLine(line);

        qDebug() << "Sensors:";
        for (int i = 0; i < packet.sensors.size(); ++i) {
            double v = packet.sensors[i];
            sensorLine += QString("Sensor %1: %2    ").arg(i).arg(v, 0, 'f', 2);
        }

        ui->sensorData->setText(sensorLine.trimmed() + "\n");


        if (!packet.log.isEmpty()) {
            qDebug() << "Log:" << packet.log;
            ui->console->append(packet.log);
        }

    }
}


void MainWindow::setupSerial()
{
    serial = new QSerialPort(this);
    serial->setPortName("/dev/pts/5"); // <-- Zmien na swój port
    serial->setBaudRate(QSerialPort::Baud9600);

    if (serial->open(QIODevice::ReadOnly)) {
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
        ui->console->append("Port otwarty.");
    } else {
        ui->console->append("Nie można otworzyć portu.");
    }
}


ParsedPacket parseSerialLine(const QString& line)
{
    ParsedPacket result;

    QString cleanLine = line.trimmed();

    int logIndex = cleanLine.indexOf('!');

    QString dataPart = logIndex >= 0 ? cleanLine.left(logIndex) : cleanLine;
    QString logPart = logIndex >= 0 ? cleanLine.mid(logIndex + 1) : "";

    QStringList items = dataPart.split(';', Qt::SkipEmptyParts);
    for (const QString& item : items) {
        bool ok;
        double val = item.toDouble(&ok);
        if (ok)
            result.sensors.append(val);
    }

    result.log = logPart;
    return result;
}



/**
 * @brief Konstruktor klasy MainWindow.
 *
 * Inicjalizuje interfejs użytkownika, ustawia tytuł okna, tworzy timer do aktualizacji logu i danych z czujników,
 * oraz ładuje mapę do widoku graficznego.
 *
 * @param parent Wskaźnik na rodzica QWidget (opcjonalny).
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Project FOKA ground base");

    logTimer = new QTimer(this);

    QTimer::singleShot(0, this, &MainWindow::setupSerial);

    // Co sekundę (1000 ms) wykonuj logStep() i updateSensorData()
    // connect(logTimer, &QTimer::timeout, this, &MainWindow::logStep);
    // connect(logTimer, &QTimer::timeout, this, &MainWindow::updateSensorData);
    logTimer->start(1000); // 1000 ms = 1 sekunda

    // Tworzymy scenę
    QGraphicsScene *scene = new QGraphicsScene(this);

    // Wczytujemy obrazek mapy
    QPixmap pixmap("/home/antek/Downloads/map.jpg");
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);

    // Dodajemy do sceny i podpinamy do widoku
    scene->addItem(item);
    ui->map->setScene(scene);
}

/**
 * @brief Destruktor klasy MainWindow.
 *
 * Usuwa wskaźnik na interfejs UI.
 */
MainWindow::~MainWindow()
{
    delete ui;
}
