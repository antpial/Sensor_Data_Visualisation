#ifndef PARSEDPACKET_H
#define PARSEDPACKET_H

#include <QVector>
#include <QString>

/**
 * @struct ParsedPacket
 * @brief Struktura reprezentująca paczkę danych odebraną z portu szeregowego.
 */
struct ParsedPacket {
    QVector<double> sensors;
    QString log;
};

#endif // PARSEDPACKET_H
