#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>

#include "parsedpacket.h"

class SerialManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialManager(QObject *parent = nullptr);
    void start(const QString &portName);

signals:
    void newPacketReceived(const ParsedPacket &packet);
    void serialError(const QString &error);

private slots:
    void readData();

private:
    ParsedPacket parseSerialLine(const QString &line);
    QSerialPort *serial;
};

#endif // SERIALMANAGER_H
