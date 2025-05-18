#include "mapwindow.h"
#include <QQmlContext>
#include <QVBoxLayout>
#include <QQuickItem>

MapWindow::MapWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // setWindowTitle("Mapa łódki");
    // resize(800, 600);

    // quickWidget = new QQuickWidget(this);
    // quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // quickWidget->setSource(QUrl::fromLocalFile("/home/antek/WDSv2/mapa.qml"));
    // setCentralWidget(quickWidget);
}

void MapWindow::updatePosition(double latitude, double longitude)
{
    QObject *rootObject = quickWidget->rootObject();
    if (!rootObject) {
        qWarning() << "Root object is null!";
        return;
    }

    QVariant lat = latitude;
    QVariant lon = longitude;

    bool invoked = QMetaObject::invokeMethod(rootObject, "updateBoatPosition",
                                             Q_ARG(QVariant, lat),
                                             Q_ARG(QVariant, lon));
    if (!invoked) {
        qWarning() << "Failed to invoke updateBoatPosition!";
    }
}

