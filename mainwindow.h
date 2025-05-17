#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include "serialmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct SensorTile {
    QFrame* frame;
    QLabel* nameLabel;
    QLabel* valueLabel;
};

class ChartWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleNewPacket(const ParsedPacket &packet);
    void handleSerialError(const QString &error);

private:
    Ui::MainWindow *ui;
    QTimer *logTimer;
    SerialManager *serialManager;
    ChartWindow *chartWindow;
    QVector<QLabel*> sensorLabels;
    QVector<SensorTile> sensorTiles;
    QVector<QPair<QString, QString>> sensorConfig;
    QVector<QPair<QString, QString>> loadSensorConfig(const QString&);


};

#endif // MAINWINDOW_H
