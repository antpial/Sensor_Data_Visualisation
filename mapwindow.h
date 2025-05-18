#pragma once

#include <QMainWindow>
#include <QtPositioning/QGeoCoordinate>
#include <QQuickWidget>

class MapWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWindow(QWidget *parent = nullptr);
    void updatePosition(double latitude, double longitude);

private:
    QQuickWidget *quickWidget;
};
