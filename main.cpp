/**
 * @file main.cpp
 * @brief Punkt wejścia do aplikacji Project FOKA.
 *
 * Inicjalizuje aplikację Qt, tworzy i wyświetla główne okno MainWindow.
 */

#include "mainwindow.h"
#include <QApplication>

/**
 * @brief Główna funkcja uruchamiająca aplikację Qt.
 * @param argc Liczba argumentów wiersza poleceń.
 * @param argv Tablica argumentów wiersza poleceń.
 * @return Kod zakończenia aplikacji.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
