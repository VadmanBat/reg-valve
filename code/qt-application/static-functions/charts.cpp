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

Application::Pair Application::computeAxesRange(double min, double max) {
    const double range = max - min;
    std::cout << min << ' ';
    std::cout << max << " = ";
    std::cout << (std::abs(min) < 1e-3 * range ? 0 : min - 0.05 * range) << ' ';
    std::cout << (std::abs(max) < 1e-3 * range ? 0 : max + 0.05 * range) << '\n';
    return {
        std::abs(min) < 1e-3 * range ? 0 : min - 0.05 * range,
        std::abs(max) < 1e-3 * range ? 0 : max + 0.05 * range
    };

}

void Application::updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y) {
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

    if (newAxisX) {
        newAxisX->setTitleText(oldAxisXTitle);
        newAxisX->setRange(range_x.first, range_x.second);
    }
    if (newAxisY) {
        newAxisY->setTitleText(oldAxisYTitle);
        newAxisY->setRange(range_y.first, range_y.second);
    }
}