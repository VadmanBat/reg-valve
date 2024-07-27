//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

QLayout* Application::createCharts(const ChartsDataset& charts, QWidget* tab) {
    auto layout = new QHBoxLayout;
    for (auto& [chart, title, titleX, titleY] : charts) {
        chart->setTitle(title);
        createAxes(chart, titleX, titleY);
        layout->addWidget(new QChartView(chart, tab));
    }
    return layout;
}

void Application::eraseLastSeries(QChart* chart) {
    if (chart->series().empty())
        return;

    chart->removeSeries(chart->series().back());
    chart->update();
}

void Application::removeAllSeries(QChart* chart) {
    chart->removeAllSeries();
    chart->update();
}

void Application::createAxes(QChart* chart, const QString& titleX, const QString& titleY) {
    auto axisX = new QValueAxis(chart);
    auto axisY = new QValueAxis(chart);

    axisX->setTitleText(titleX);
    axisY->setTitleText(titleY);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
}

void Application::updateAxes(QChart* chart) {
    QString oldAxisXTitle, oldAxisYTitle;

    auto oldAxisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    auto oldAxisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());

    if (oldAxisX) {
        oldAxisXTitle = oldAxisX->titleText();
        chart->removeAxis(oldAxisX);
    }
    if (oldAxisY) {
        oldAxisYTitle = oldAxisY->titleText();
        chart->removeAxis(oldAxisY);
    }

    chart->createDefaultAxes();

    auto newAxisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    auto newAxisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());

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