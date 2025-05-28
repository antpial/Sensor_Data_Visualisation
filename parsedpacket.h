/**
 * @file parsedpacket.h
 * @brief Deklaracja struktury ParsedPacket służącej do przechowywania danych odebranych z portu szeregowego.
 */

#ifndef PARSEDPACKET_H
#define PARSEDPACKET_H

#include <QVector>
#include <QString>

/**
 * @struct ParsedPacket
 * @brief Struktura reprezentująca paczkę danych odebraną z portu szeregowego.
 *
 * Przechowuje aktualne odczyty sensorów, informacje do logów oraz dane lokalizacyjne (jeśli obecne).
 */
struct ParsedPacket {
    QVector<double> sensors;   ///< Wektor wartości odczytanych z sensorów.
    QString log;               ///< Tekst do wyświetlenia w konsoli (log systemowy).
    bool hasPosition;          ///< Flaga wskazująca, czy pakiet zawiera dane GPS.
    double latitude;           ///< Szerokość geograficzna (jeśli hasPosition == true).
    double longitude;          ///< Długość geograficzna (jeśli hasPosition == true).
};

#endif // PARSEDPACKET_H
