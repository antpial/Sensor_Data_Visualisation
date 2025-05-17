#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "serialmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
};

#endif // MAINWINDOW_H
