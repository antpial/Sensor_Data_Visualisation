/**
 * @file chartwindow.h
 * @brief Deklaracja klasy ChartWindow odpowiedzialnej za wyświetlanie wykresów sensorów.
 */

#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>

#include "parsedpacket.h"

QT_CHARTS_USE_NAMESPACE

    namespace Ui {
    class ChartWindow;
}

/**
 * @brief Klasa ChartWindow reprezentuje okno wykresów w aplikacji Project FOKA.
 *
 * Wyświetla dane z sensorów w formie wykresów liniowych.
 * Każdy sensor ma swój osobny wykres, aktualizowany przy nadejściu nowego pakietu danych.
 */
class ChartWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy ChartWindow.
     * @param parent Wskaźnik na obiekt nadrzędny (domyślnie nullptr).
     */
    explicit ChartWindow(QWidget *parent = nullptr);

    /**
     * @brief Umożliwia przetłumaczenie interfejsu wykresów po zmianie języka aplikacji.
     */
    void retranslateUi();

    /**
     * @brief Destruktor klasy ChartWindow.
     */
    ~ChartWindow();

public slots:
    /**
     * @brief Aktualizuje dane wykresów na podstawie odebranego pakietu sensorów.
     * @param packet Struktura z danymi odebranymi z portu szeregowego.
     */
    void updateFromPacket(const ParsedPacket &packet);

private:
    Ui::ChartWindow *ui;                  ///< Wskaźnik na interfejs użytkownika (wygenerowany przez Qt Designer)
    QList<QLineSeries*> seriesList;       ///< Lista serii danych (jedna seria na jeden wykres/sensor)

    /**
     * @brief Inicjalizuje wykresy zgodnie z konfiguracją z pliku JSON.
     * Tworzy wykresy, ustawia ich zakresy i kolory.
     */
    void initializeCharts();
};

#endif // CHARTWINDOW_H
