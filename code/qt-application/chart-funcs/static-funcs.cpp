//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

void GraphWindow::removeAllSeries(QChart *chart) {
    chart->removeAllSeries();
    chart->update();
}

void GraphWindow::createAxes(QChart *chart,
                             const QString& titleX,
                             const QString& titleY)
{
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    axisX->setTitleText(titleX);
    axisY->setTitleText(titleY);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
}

void GraphWindow::updateAxes(QChart *chart) {
    QString oldAxisXTitle, oldAxisYTitle;

    QValueAxis *oldAxisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    QValueAxis *oldAxisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());

    if (oldAxisX) {
        oldAxisXTitle = oldAxisX->titleText();
        chart->removeAxis(oldAxisX);
    }
    if (oldAxisY) {
        oldAxisYTitle = oldAxisY->titleText();
        chart->removeAxis(oldAxisY);
    }

    chart->createDefaultAxes();

    QValueAxis *newAxisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    QValueAxis *newAxisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());

    if (newAxisX)
        newAxisX->setTitleText(oldAxisXTitle);
    if (newAxisY) {
        newAxisY->setTitleText(oldAxisYTitle);

        qreal minY = newAxisY->min();
        qreal maxY = newAxisY->max();
        qreal range = maxY - minY;
        qreal newMinY = minY - 0.05 * range;
        qreal newMaxY = maxY + 0.05 * range;

        /*const int order = std::floor(std::log10((minY + maxY) / 2));
        qreal scale = std::pow(10, order - 1);
        qreal roundedMinY = std::ceil(newMinY / scale) * scale;
        qreal roundedMaxY = std::ceil(newMaxY / scale) * scale;*/

        newAxisY->setRange(newMinY, newMaxY);
    }
}