/**
 * @file mainwindow.h
 * @brief Deklaracja klasy MainWindow - głównego interfejsu użytkownika aplikacji Project FOKA.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QTranslator>
#include "serialmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Struktura reprezentująca pojedynczy kafelek sensora w GUI.
 */
struct SensorTile {
    QFrame* frame;         ///< Obiekt ramki dla sensora
    QLabel* nameLabel;     ///< Etykieta z nazwą sensora
    QLabel* valueLabel;    ///< Etykieta z aktualną wartością sensora
};

class ChartWindow;

/**
 * @brief Klasa reprezentująca główne okno aplikacji Project FOKA.
 * Zarządza UI, mapą, portem szeregowym, obsługą sensorów i lokalizacją łodzi.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * @param parent Wskaźnik na widget nadrzędny (domyślnie nullptr).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Obsługuje nowy pakiet danych odebrany z portu szeregowego.
     * @param packet Struktura z danymi pomiarowymi i lokalizacją.
     */
    void handleNewPacket(const ParsedPacket &packet);

    /**
     * @brief Obsługuje błędy portu szeregowego.
     * @param error Treść błędu.
     */
    void handleSerialError(const QString &error);

private:
    Ui::MainWindow *ui;                            ///< Interfejs użytkownika wygenerowany przez Qt Designer
    QTimer *logTimer;                              ///< Timer do obsługi logów (obecnie nieużywany)
    SerialManager *serialManager;                  ///< Manager portu szeregowego
    ChartWindow *chartWindow;                      ///< Okno wykresów
    QVector<QLabel*> sensorLabels;                 ///< Etykiety sensorów (nieużywane, zachowane na przyszłość)
    QVector<SensorTile> sensorTiles;               ///< Kafelki sensorów
    QVector<QPair<QString, QString>> sensorConfig; ///< Konfiguracja sensorów: para <nazwa, jednostka>

    /**
     * @brief Ładuje konfigurację sensorów z pliku JSON.
     * @param filePath Ścieżka do pliku JSON z konfiguracją.
     * @return Wektor par <nazwa, jednostka>.
     */
    QVector<QPair<QString, QString>> loadSensorConfig(const QString &filePath);

    /**
     * @brief Aktualizuje pozycję łodzi na mapie.
     * @param latitude Szerokość geograficzna.
     * @param longitude Długość geograficzna.
     */
    void updatePosition(double latitude, double longitude);

    // Przyciski językowe
    QPushButton *langButton = nullptr; ///< Przyciski zmiany języka (deprecated / nieużywane)
    QPushButton *btnPL = nullptr;      ///< Przycisk do wyboru języka polskiego
    QPushButton *btnEN = nullptr;      ///< Przycisk do wyboru języka angielskiego
    QTranslator translator;            ///< Tłumacz Qt

    /**
     * @brief Ustawia pozycję przycisków językowych względem mapy.
     */
    void updateLanguageButtonPositions();

    /**
     * @brief Ładuje i stosuje tłumaczenie GUI.
     * @param langCode Kod języka (np. "pl", "en").
     */
    void loadLanguage(const QString &langCode);

    /**
     * @brief Ustawia mapę i jej parametry (funkcja nieużywana).
     */
    void setupMap();

    // Dane lokalizacyjne
    bool mapReady = false;             ///< Flaga gotowości mapy
    bool hasPendingPosition = false;   ///< Flaga oczekującej pozycji do narysowania
    double lastLat = 0;                ///< Ostatnia szerokość geograficzna
    double lastLon = 0;                ///< Ostatnia długość geograficzna

protected:
    /**
     * @brief Obsługuje zdarzenie zmiany rozmiaru okna.
     * @param event Obiekt zdarzenia zmiany rozmiaru.
     */
    void resizeEvent(QResizeEvent *event) override;
};

#endif // MAINWINDOW_H
