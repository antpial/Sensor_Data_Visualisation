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
#include <QPushButton>
#include <QResizeEvent>
#include <QTranslator>

#define PORT_PATH "/dev/pts/2"
#define CRITICAL_DEPTH 50


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Project FOKA ground base"));
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
    ui->map->setSource(QUrl::fromLocalFile("/home/antek/WDSv2/mapa.qml"));
    ui->map->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // PL przycisk
    btnPL = new QPushButton(this);
    btnPL->setIcon(QIcon("/home/antek/WDSv2/icons/pl.png"));
    btnPL->setIconSize(QSize(32, 32));
    btnPL->setToolTip("Polski");
    btnPL->resize(30, 30);
    btnPL->setStyleSheet("border: none;");  // opcjonalnie, bez ramki
    connect(btnPL, &QPushButton::clicked, this, [=]() {
        loadLanguage("pl");
    });

    // GB przycisk
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

    // ustaw pozycje po załadowaniu GUI
    QTimer::singleShot(0, this, [this]() {
        updateLanguageButtonPositions();
    });




    //tabliczki z sensorami
    sensorConfig = loadSensorConfig("charts_config.json"); // ścieżka do pliku JSON

    // sposob zeby przycisk odrazu sie dobrze ustawial
    QTimer::singleShot(0, this, [this]() {
        resizeEvent(nullptr);  // wywoła ustawienie pozycji przycisku
    });

}


void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (btnPL && ui->map) {
        QPoint mapTopRight = ui->map->geometry().topRight();
        int x = mapTopRight.x() - btnPL->width() ;
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

void MainWindow::updateLanguageButtonPositions() {
    if (!ui->map) return;

    QPoint mapTopRight = ui->map->geometry().topRight();
    int margin = 10;

    // 🇬🇧 po prawej
    if (btnEN)
        btnEN->move(mapTopRight.x() - btnEN->width() - margin, mapTopRight.y() + margin);

    // 🇵🇱 obok 🇬🇧
    if (btnPL && btnEN)
        btnPL->move(btnEN->x() - btnPL->width() - 5, btnEN->y());
}

void MainWindow::loadLanguage(const QString &langCode) {
    qApp->removeTranslator(&translator);

    if (translator.load("/home/antek/WDSv2/translations/translations_" + langCode + ".qm")) {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);  // automatycznie przetłumaczy widżety z .ui
        chartWindow->retranslateUi();
    } else {
        qDebug() << "Nie udało się załadować tłumaczenia:" << langCode;
    }
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

            QLabel* nameLabel = new QLabel(tr(qUtf8Printable(name)), frame);
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
        lastLat = packet.latitude;
        lastLon = packet.longitude;
        hasPendingPosition = true;

        if (mapReady) {
            updatePosition(lastLat, lastLon);
            hasPendingPosition = false;
        }
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
