#ifndef PARSEDPACKET_H
#define PARSEDPACKET_H

#include <QVector>
#include <QString>

/**
 * @struct ParsedPacket
 * @brief Struktura reprezentująca paczkę danych odebraną z portu szeregowego.
 */
struct ParsedPacket {
    QVector<double> sensors;    // odczyty z sensorów
    QString log;                // ewentualny tekst do logu (console)
    bool hasPosition;          // flaga, czy pakiet zawiera dane pozycyjne
    double latitude;           // szerokość geograficzna
    double longitude;          // długość geograficzna
};


#endif // PARSEDPACKET_H
