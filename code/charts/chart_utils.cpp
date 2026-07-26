#include "code/charts/chart_utils.hpp"

#include "code/dialogs/chart_dialog.h"
#include "code/series/complex_series.hpp"
#include "code/series/series.hpp"
#include "code/util/data_file_parser.hpp"

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QGuiApplication>
#include <QLineSeries>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QScreen>
#include <QTextStream>
#include <QValueAxis>

#include <algorithm>

#define CHART_TR(str) QCoreApplication::translate("chart_utils", str)

#include <cmath>

namespace chart_utils {
namespace {

QList<QPointF> toPoints(const VecPair& data) {
    QList<QPointF> pts;
    pts.reserve(static_cast<int>(data.size()));
    for (const auto& [x, y] : data)
        pts.append(QPointF(x, y));
    return pts;
}

QList<QPointF> toPoints(const VecComp& data) {
    QList<QPointF> pts;
    pts.reserve(static_cast<int>(data.size()));
    for (const auto& z : data)
        pts.append(QPointF(z.real(), z.imag()));
    return pts;
}

} // namespace

QPen penForIndex(std::size_t index) {
    static const QPen pens[6] = {
        QPen(QColor(0x1f, 0x77, 0xb4), 2), QPen(QColor(0xff, 0x7f, 0x0e), 2), QPen(QColor(0x2c, 0xa0, 0x2c), 2),
        QPen(QColor(0xd6, 0x27, 0x28), 2), QPen(QColor(0x94, 0x67, 0xbd), 2), QPen(QColor(0x8c, 0x56, 0x4b), 2),
    };
    return pens[index % 6];
}

void createAxes(QChart* chart, const QString& titleX, const QString& titleY) {
    auto* axisX = new QValueAxis(chart);
    auto* axisY = new QValueAxis(chart);
    axisX->setTitleText(titleX);
    axisY->setTitleText(titleY);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
}

void createChartContextMenu(QChartView* chartView) {
    auto* saveImageAction  = new QAction(CHART_TR("Сохранить как PNG"), chartView);
    auto* saveTextAction   = new QAction(CHART_TR("Сохранить как TXT"), chartView);
    auto* copyAction       = new QAction(CHART_TR("Копировать изображение"), chartView);
    auto* propertiesAction = new QAction(CHART_TR("Свойства"), chartView);

    QChart* chart = chartView->chart();

    QObject::connect(saveImageAction, &QAction::triggered, [chart, chartView] {
        const QString fileName =
            QFileDialog::getSaveFileName(nullptr, CHART_TR("Сохранить график"), chart->title(),
                                         CHART_TR("Рисунок PNG (*.png);;Все файлы (*)"));
        if (!fileName.isEmpty() && !chartView->grab().save(fileName, "png"))
            QMessageBox::warning(nullptr, CHART_TR("Ошибка"), CHART_TR("Не удалось сохранить график!"));
    });

    QObject::connect(saveTextAction, &QAction::triggered, [chart] {
        const QString fileName =
            QFileDialog::getSaveFileName(nullptr, CHART_TR("Сохранить график"), chart->title(),
                                         CHART_TR("Текст txt (*.txt);;Все файлы (*)"));
        if (!fileName.isEmpty() && !saveChartToFile(fileName, chart))
            QMessageBox::warning(nullptr, CHART_TR("Ошибка"), CHART_TR("Не удалось сохранить график!"));
    });

    QObject::connect(copyAction, &QAction::triggered, [chartView] {
        QApplication::clipboard()->setImage(chartView->grab().toImage());
    });

    QObject::connect(propertiesAction, &QAction::triggered, [chart] {
        ChartDialog dialog(chart);
        dialog.exec();
    });

    auto* contextMenu = new QMenu(chartView);
    contextMenu->addAction(saveImageAction);
    contextMenu->addAction(saveTextAction);
    contextMenu->addAction(copyAction);
    contextMenu->addSeparator();
    contextMenu->addAction(propertiesAction);

    chartView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(chartView, &QChartView::customContextMenuRequested, [contextMenu, chartView](const QPoint& pos) {
        contextMenu->exec(chartView->mapToGlobal(pos));
    });
}

void eraseLastSeries(QChart* chart) {
    const auto all = chart->series();
    for (auto i = all.size(); i > 0; --i) {
        auto* s = all[i - 1];
        if (s->name() == QLatin1String("hor-line") || s->name() == QLatin1String("ver-line"))
            continue;
        chart->removeSeries(s);
        delete s;
        chart->update();
        return;
    }
}

void removeAllSeries(QChart* chart) {
    for (auto* series : chart->series()) {
        if (series->name() == QLatin1String("hor-line") || series->name() == QLatin1String("ver-line"))
            continue;
        chart->removeSeries(series);
        delete series;
    }
    chart->update();
}

Pair computeAxesRange(double min, double max) {
    const double range = max - min;
    if (range <= 0)
        return {min - 1, max + 1};
    return {std::abs(min) < 1e-3 * range ? 0 : min - 0.05 * range,
            std::abs(max) < 1e-3 * range ? 0 : max + 0.05 * range};
}

void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y) {
    auto* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (!axisX || !axisY)
        return;
    axisX->setRange(range_x.first, range_x.second);
    axisY->setRange(range_y.first, range_y.second);
    for (auto* series : chart->series()) {
        if (!series->attachedAxes().contains(axisX))
            series->attachAxis(axisX);
        if (!series->attachedAxes().contains(axisY))
            series->attachAxis(axisY);
    }
}

void addRealSeries(QChart* chart, const VecPair& points, const QString& title, std::size_t index,
                   bool useOptimizedSubset) {
    if (points.empty())
        return;
    auto* series = new QLineSeries;
    series->setName(title);
    series->setPen(penForIndex(index));
    if (useOptimizedSubset && points.size() > 64) {
        Series opt(points);
        series->replace(toPoints(opt.optimal()));
    } else {
        series->replace(toPoints(points));
    }
    chart->addSeries(series);
}

void addComplexSeries(QChart* chart, const VecComp& points, const QString& title, std::size_t index,
                      bool useOptimizedSubset) {
    if (points.empty())
        return;
    auto* series = new QLineSeries;
    series->setName(title);
    series->setPen(penForIndex(index));
    if (useOptimizedSubset && points.size() > 64) {
        ComplexSeries opt(points);
        series->replace(toPoints(opt.optimal()));
    } else {
        series->replace(toPoints(points));
    }
    chart->addSeries(series);
}

namespace {

QLineSeries* lastDataSeries(QChart* chart) {
    if (!chart)
        return nullptr;
    const auto all = chart->series();
    for (auto i = all.size(); i > 0; --i) {
        auto* s = qobject_cast<QLineSeries*>(all[i - 1]);
        if (!s)
            continue;
        if (s->name() == QLatin1String("hor-line") || s->name() == QLatin1String("ver-line"))
            continue;
        return s;
    }
    return nullptr;
}

QList<QPointF> pointsForReal(const VecPair& points, bool useOptimizedSubset) {
    if (useOptimizedSubset && points.size() > 64) {
        Series opt(points);
        return toPoints(opt.optimal());
    }
    return toPoints(points);
}

QList<QPointF> pointsForComplex(const VecComp& points, bool useOptimizedSubset) {
    if (useOptimizedSubset && points.size() > 64) {
        ComplexSeries opt(points);
        return toPoints(opt.optimal());
    }
    return toPoints(points);
}

} // namespace

bool replaceLastRealSeries(QChart* chart, const VecPair& points, const QString& title,
                           bool useOptimizedSubset) {
    auto* series = lastDataSeries(chart);
    if (!series || points.empty())
        return false;
    series->setName(title);
    series->replace(pointsForReal(points, useOptimizedSubset));
    return true;
}

bool replaceLastComplexSeries(QChart* chart, const VecComp& points, const QString& title,
                              bool useOptimizedSubset) {
    auto* series = lastDataSeries(chart);
    if (!series || points.empty())
        return false;
    series->setName(title);
    series->replace(pointsForComplex(points, useOptimizedSubset));
    return true;
}

double displayRefreshHz() {
    qreal hz = 60.0;
    if (QScreen* screen = QGuiApplication::primaryScreen()) {
        const qreal r = screen->refreshRate();
        if (r >= 30.0 && r <= 500.0)
            hz = r;
    }
    return static_cast<double>(hz);
}

int frameIntervalMs() {
    const double hz = displayRefreshHz();
    return std::max(1, static_cast<int>(std::lround(1000.0 / hz)));
}

bool saveChartToFile(const QString& fileName, QChart* chart) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    for (QAbstractSeries* series : chart->series()) {
        if (series->name() == QLatin1String("hor-line") || series->name() == QLatin1String("ver-line"))
            continue;
        out << "Name: " << series->name() << '\n';
        if (auto* xy = qobject_cast<QXYSeries*>(series)) {
            for (int i = 0; i < xy->count(); ++i)
                out << xy->at(i).x() << ", " << xy->at(i).y() << '\n';
        }
        out << '\n';
    }
    return true;
}

VecPair readVectorFromFile(const QString& fileName) {
    // Robust path: '.' / ',' decimals, junk separators stripped (see data_file_parser).
    auto opt = data_file_parser::readStepResponse(fileName);
    return opt.value_or(VecPair{});
}

QChartView* makeChartView(QChart* chart, QWidget* parent, const QString& title, const QString& titleX,
                          const QString& titleY) {
    chart->setTitle(title);
    // Instant redraw on data replace (no Qt Charts tweening).
    chart->setAnimationOptions(QChart::NoAnimation);
    createAxes(chart, titleX, titleY);
    auto* view = new QChartView(chart, parent);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    createChartContextMenu(view);
    return view;
}

} // namespace chart_utils
