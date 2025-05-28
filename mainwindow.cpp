/**
 * @file mainwindow.cpp
 * @brief Implementacja klasy MainWindow - głównego okna aplikacji naziemnej Project FOKA.
 */

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
#include <QQmlContext>
#include <QVBoxLayout>
#include <QQuickItem>
#include <QPushButton>
#include <QResizeEvent>
#include <QTranslator>

#define PORT_PATH "/dev/pts/1"           ///< Ścieżka do portu szeregowego
#define CRITICAL_DEPTH 50                ///< Głębokość krytyczna w cm

/**
 * @brief Konstruktor klasy MainWindow.
 * Inicjalizuje UI, komponenty graficzne, managera portu szeregowego oraz ustawia pozycje elementów.
 * @param parent Wskaźnik na obiekt nadrzędny (domyślnie nullptr).
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Project FOKA ground base"));
    this->setStyleSheet("background-color: #ffffff;");
    ui->console->setStyleSheet("background-color: #ffffff;");
    ui->sensorBar->setStyleSheet("background-color: #ffffff");

    // Inicjalizacja menedżera portu szeregowego
    serialManager = new SerialManager(this);
    connect(serialManager, &SerialManager::newPacketReceived, this, &MainWindow::handleNewPacket);
    connect(serialManager, &SerialManager::serialError, this, &MainWindow::handleSerialError);
    serialManager->start(PORT_PATH);

    // Okno wykresów
    chartWindow = new ChartWindow();
    chartWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(serialManager, &SerialManager::newPacketReceived, chartWindow, &ChartWindow::updateFromPacket);
    chartWindow->show();

    // Inicjalizacja mapy
    ui->map->setSource(QUrl::fromLocalFile("/home/antek/WDSv2/mapa.qml"));
    ui->map->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Przyciski językowe
    btnPL = new QPushButton(this);
    btnPL->setIcon(QIcon("/home/antek/WDSv2/icons/pl.png"));
    btnPL->setIconSize(QSize(32, 32));
    btnPL->setToolTip("Polski");
    btnPL->resize(30, 30);
    btnPL->setStyleSheet("border: none;");
    connect(btnPL, &QPushButton::clicked, this, [=]() {
        loadLanguage("pl");
    });

    btnEN = new QPushButton(this);
    btnEN->setIcon(QIcon("/home/antek/WDSv2/icons/gb.png"));
    btnEN->setIconSize(QSize(32, 32));
    btnEN->setToolTip("English");
    btnEN->resize(30, 30);
    btnEN->setStyleSheet("border: none;");
    btnEN->show();
    connect(btnEN, &QPushButton::clicked, this, [=]() {
        loadLanguage("en");
    });

    QTimer::singleShot(0, this, [this]() {
        updateLanguageButtonPositions();
    });

    sensorConfig = loadSensorConfig("charts_config.json");

    QTimer::singleShot(0, this, [this]() {
        resizeEvent(nullptr);
    });
}

/**
 * @brief Zdarzenie zmiany rozmiaru okna.
 * @param event Obiekt zdarzenia (może być nullptr).
 */
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (btnPL && ui->map) {
        QPoint mapTopRight = ui->map->geometry().topRight();
        int x = mapTopRight.x() - btnPL->width();
        int y = mapTopRight.y() + 20;
        btnPL->move(x, y);
    }

    if (btnEN && ui->map) {
        QPoint mapTopRight = ui->map->geometry().topRight();
        int x = mapTopRight.x() - btnEN->width() - 35;
        int y = mapTopRight.y() + 20;
        btnEN->move(x, y);
    }
}

/**
 * @brief Ustawia pozycję przycisków językowych względem mapy.
 */
void MainWindow::updateLanguageButtonPositions() {
    if (!ui->map) return;

    QPoint mapTopRight = ui->map->geometry().topRight();
    int margin = 10;

    if (btnEN)
        btnEN->move(mapTopRight.x() - btnEN->width() - margin, mapTopRight.y() + margin);
    if (btnPL && btnEN)
        btnPL->move(btnEN->x() - btnPL->width() - 5, btnEN->y());
}

/**
 * @brief Ładuje tłumaczenie interfejsu na podstawie kodu języka.
 * @param langCode Kod języka (np. "pl", "en").
 */
void MainWindow::loadLanguage(const QString &langCode) {
    qApp->removeTranslator(&translator);

    if (translator.load("/home/antek/WDSv2/translations/translations_" + langCode + ".qm")) {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        chartWindow->retranslateUi();
    } else {
        qDebug() << "Nie udało się załadować tłumaczenia:" << langCode;
    }
}

/**
 * @brief Destruktor klasy MainWindow.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Ładuje konfigurację sensorów z pliku JSON.
 * @param filePath Ścieżka do pliku JSON.
 * @return Wektor par <nazwa, jednostka>.
 */
QVector<QPair<QString, QString>> MainWindow::loadSensorConfig(const QString &filePath)
{
    QVector<QPair<QString, QString>> sensors;

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

/**
 * @brief Obsługuje nowy pakiet danych odebrany z portu szeregowego.
 * @param packet Struktura zawierająca dane pomiarowe i pozycję.
 */
void MainWindow::handleNewPacket(const ParsedPacket &packet)
{
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->sensorBar->layout());
    if (!layout) return;

    if (sensorTiles.size() != packet.sensors.size()) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        sensorTiles.clear();

        for (int i = 0; i < packet.sensors.size(); ++i) {
            QFrame* frame = new QFrame(this);
            frame->setFrameShape(QFrame::StyledPanel);
            frame->setMaximumWidth(150);
            frame->setMinimumHeight(60);
            frame->setMaximumHeight(120);
            frame->setStyleSheet("QFrame { border: 1px solid #000000; border-radius: 6px; background-color: #ffffff; }");

            QVBoxLayout* vbox = new QVBoxLayout(frame);

            QString name = (i < sensorConfig.size()) ? sensorConfig[i].first : QString("Sensor %1").arg(i);
            QString unit = (i < sensorConfig.size()) ? sensorConfig[i].second : "";

            QLabel* nameLabel = new QLabel(tr(qUtf8Printable(name)), frame);
            nameLabel->setAlignment(Qt::AlignCenter);
            nameLabel->setStyleSheet("font-size: 20px; border: 0px;");

            QLabel* valueLabel = new QLabel("0.00 " + unit, frame);
            valueLabel->setAlignment(Qt::AlignCenter);
            valueLabel->setStyleSheet("font-size: 20px; border: 0px;");

            vbox->addWidget(nameLabel);
            vbox->addWidget(valueLabel);

            layout->addWidget(frame);
            sensorTiles.append({frame, nameLabel, valueLabel});
        }
    }

    for (int i = 0; i < packet.sensors.size(); ++i) {
        QString unit = (i < sensorConfig.size()) ? sensorConfig[i].second : "";
        sensorTiles[i].valueLabel->setText(QString::number(packet.sensors[i], 'f', 2) + " " + unit);

        if (sensorTiles[i].nameLabel->text() == "Depth") {
            if (packet.sensors[i] < CRITICAL_DEPTH) {
                sensorTiles[i].frame->setStyleSheet("QFrame { background-color: #f52f2f; border-radius: 6px; border: 1px solid #000000; }");
            } else {
                sensorTiles[i].frame->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 6px; border: 1px solid #000000; }");
            }
        }
    }

    if (!packet.log.isEmpty()) {
        ui->console->append(packet.log);
    }

    if (packet.hasPosition) {
        lastLat = packet.latitude;
        lastLon = packet.longitude;
        hasPendingPosition = true;
        updatePosition(lastLat, lastLon);
        hasPendingPosition = false;
    }
}

/**
 * @brief Aktualizuje pozycję łodzi na mapie.
 * @param latitude Szerokość geograficzna.
 * @param longitude Długość geograficzna.
 */
void MainWindow::updatePosition(double latitude, double longitude)
{
    QObject *rootObject = ui->map->rootObject();
    if (!rootObject) {
        qWarning() << "Root object is null!";
        return;
    }

    bool invoked = QMetaObject::invokeMethod(rootObject, "updateBoatPosition",
                                             Q_ARG(QVariant, latitude),
                                             Q_ARG(QVariant, longitude));
    if (!invoked) {
        qWarning() << "Failed to invoke updateBoatPosition!";
    }
}

/**
 * @brief Obsługuje błędy związane z komunikacją szeregową.
 * @param error Komunikat błędu.
 */
void MainWindow::handleSerialError(const QString &error)
{
    ui->console->append("Błąd: " + error);
}
