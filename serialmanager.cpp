#include "serialmanager.h"
#include <QDebug>

SerialManager::SerialManager(QObject *parent)
    : QObject(parent), serial(new QSerialPort(this))
{
    connect(serial, &QSerialPort::readyRead, this, &SerialManager::readData);
}

void SerialManager::start(const QString &portName)
{
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    if (!serial->open(QIODevice::ReadOnly)) {
        emit serialError("!Nie można otworzyć portu: " + portName);
    }
}

void SerialManager::readData()
{
    while (serial->canReadLine()) {
        QByteArray data = serial->readLine();
        QString line = QString::fromUtf8(data).trimmed();
        ParsedPacket packet = parseSerialLine(line);
        emit newPacketReceived(packet);
    }
}

ParsedPacket SerialManager::parseSerialLine(const QString &line)
{
    ParsedPacket result;

    QString cleanLine = line.trimmed();
    int logIndex = cleanLine.indexOf('!');

    QString dataPart = (logIndex >= 0) ? cleanLine.left(logIndex) : cleanLine;
    QString logPart = (logIndex >= 0) ? cleanLine.mid(logIndex + 1) : "";

    QStringList items = dataPart.split(';', Qt::SkipEmptyParts);
    for (const QString &item : items) {
        bool ok;
        double val = item.toDouble(&ok);
        if (ok) result.sensors.append(val);
    }

    result.log = logPart;
    return result;
}
