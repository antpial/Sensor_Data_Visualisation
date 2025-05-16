#include "chartwindow.h"
#include "ui_chartwindow.h"
#include <QDebug>


ChartWindow::ChartWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
}

ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::on_pushButton_clicked()
{
    qDebug() << "Clicked";
}

