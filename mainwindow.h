/**
 * @file mainwindow.h
 * @brief Deklaracja klasy MainWindow – głównego okna aplikacji Project FOKA.
 *
 * Plik zawiera definicję klasy MainWindow odpowiedzialnej za logikę interfejsu użytkownika,
 * obsługę czujników i wyświetlanie mapy w aplikacji Qt.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Klasa reprezentująca główne okno aplikacji.
 *
 * MainWindow zawiera funkcjonalności takie jak wyświetlanie danych z czujników,
 * logowanie zdarzeń, oraz wizualizację mapy przy użyciu QGraphicsScene.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * @param parent Wskaźnik na nadrzędny widget (opcjonalnie).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     */
    ~MainWindow();

private:
    Ui::MainWindow *ui; ///< Interfejs użytkownika wygenerowany przez Qt Designer.
    QTimer *logTimer;   ///< Timer odpowiedzialny za okresowe wykonywanie logowania i aktualizacji danych.
    int logCounter = 0; ///< Licznik wpisów w konsoli logu.
    QSerialPort *serial;

    /**
     * @brief Dodaje nowy wpis do logu tekstowego w UI.
     */
    void logStep();

    /**
     * @brief Aktualizuje symulowane dane z czujników i wyświetla je w interfejsie.
     */
    void updateSensorData();

    void readData();

    void setupSerial();
};

#endif // MAINWINDOW_H
