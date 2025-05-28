/**
 * @file serialmanager.h
 * @brief Deklaracja klasy SerialManager odpowiedzialnej za komunikację szeregową i parsowanie danych.
 */

#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>

#include "parsedpacket.h"

/**
 * @class SerialManager
 * @brief Klasa odpowiedzialna za odbieranie danych z portu szeregowego oraz ich przetwarzanie.
 *
 * SerialManager nasłuchuje portu szeregowego, odczytuje linie danych,
 * parsuje je do struktury `ParsedPacket` i emituje sygnały w przypadku odebrania danych lub błędu.
 */
class SerialManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy SerialManager.
     * @param parent Obiekt nadrzędny (domyślnie nullptr).
     */
    explicit SerialManager(QObject *parent = nullptr);

    /**
     * @brief Uruchamia nasłuch na wskazanym porcie szeregowym.
     * @param portName Nazwa portu, np. "/dev/ttyUSB0".
     */
    void start(const QString &portName);

signals:
    /**
     * @brief Sygnał emitowany po odebraniu i sparsowaniu pakietu danych.
     * @param packet Sparsowany pakiet danych.
     */
    void newPacketReceived(const ParsedPacket &packet);

    /**
     * @brief Sygnał emitowany w przypadku błędu podczas otwierania portu.
     * @param error Treść błędu.
     */
    void serialError(const QString &error);

private slots:
    /**
     * @brief Slot do obsługi danych przychodzących z portu szeregowego.
     */
    void readData();

private:
    /**
     * @brief Parsuje linię tekstu odebraną z portu szeregowego do struktury ParsedPacket.
     * @param line Pojedyncza linia danych wejściowych.
     * @return Struktura zawierająca dane sensorów i ewentualne dane lokalizacyjne.
     */
    ParsedPacket parseSerialLine(const QString &line);

    QSerialPort *serial; ///< Obiekt do obsługi komunikacji szeregowej (QSerialPort).
};

#endif // SERIALMANAGER_H
