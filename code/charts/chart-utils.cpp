#include "code/charts/chart-utils.hpp"

#include "code/dialogs/chart-dialog.h"

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QLegend>
#include <QLegendMarker>
#include <QLineSeries>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QTextStream>
#include <QValueAxis>

#include <algorithm>

#define CHART_TR(str) QCoreApplication::translate("chart_utils", str)

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

bool isGuideSeries(const QString& name) {
    return name == QLatin1String(kHorGuide) || name == QLatin1String(kVerGuide);
}

QLineSeries* findSeriesByName(QChart* chart, const char* name) {
    if (!chart)
        return nullptr;
    for (auto* s : chart->series()) {
        if (s->name() == QLatin1String(name))
            return qobject_cast<QLineSeries*>(s);
    }
    return nullptr;
}

void hideLegendMarker(QChart* chart, QAbstractSeries* series) {
    if (!chart || !series || !chart->legend())
        return;
    for (auto* marker : chart->legend()->markers(series))
        marker->setVisible(false);
}

QPen guidePen() {
    QPen pen(QColor(0x12, 0x12, 0x12), 2.0);
    pen.setCosmetic(true);
    pen.setCapStyle(Qt::FlatCap);
    pen.setStyle(Qt::SolidLine);
    return pen;
}

QLineSeries* lastDataSeries(QChart* chart) {
    if (!chart)
        return nullptr;
    const auto all = chart->series();
    for (auto i = all.size(); i > 0; --i) {
        auto* s = qobject_cast<QLineSeries*>(all[i - 1]);
        if (!s || isGuideSeries(s->name()))
            continue;
        return s;
    }
    return nullptr;
}

void attach_to_axes(QChart* chart, QAbstractSeries* series) {
    auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (axis_x && !series->attachedAxes().contains(axis_x))
        series->attachAxis(axis_x);
    if (axis_y && !series->attachedAxes().contains(axis_y))
        series->attachAxis(axis_y);
}

} // namespace

Pair computeAxesRange(double min, double max) {
    return niceAxisRange(min, max, /*include_zero=*/true);
}

QPen penForIndex(std::size_t index) {
    static const QPen pens[6] = {
        QPen(QColor(0x1f, 0x77, 0xb4), 2), QPen(QColor(0xff, 0x7f, 0x0e), 2), QPen(QColor(0x2c, 0xa0, 0x2c), 2),
        QPen(QColor(0xd6, 0x27, 0x28), 2), QPen(QColor(0x94, 0x67, 0xbd), 2), QPen(QColor(0x8c, 0x56, 0x4b), 2),
    };
    return pens[index % 6];
}

void createAxes(QChart* chart, const QString& titleX, const QString& titleY) {
    auto* axis_x = new QValueAxis(chart);
    auto* axis_y = new QValueAxis(chart);
    axis_x->setTitleText(titleX);
    axis_y->setTitleText(titleY);
    chart->addAxis(axis_x, Qt::AlignBottom);
    chart->addAxis(axis_y, Qt::AlignLeft);
}

void updateOriginGuides(QChart* chart, const Pair& range_x, const Pair& range_y) {
    if (!chart)
        return;

    auto ensure = [chart](const char* name) -> QLineSeries* {
        auto* s = findSeriesByName(chart, name);
        if (s)
            return s;
        s = new QLineSeries;
        s->setName(QString::fromLatin1(name));
        s->setPen(guidePen());
        s->setPointsVisible(false);
        chart->addSeries(s);
        attach_to_axes(chart, s);
        hideLegendMarker(chart, s);
        return s;
    };

    auto* hor = ensure(kHorGuide);
    auto* ver = ensure(kVerGuide);

    // y = 0 — ось абсцисс (t / Re / ω)
    hor->replace(QList<QPointF>{
        {range_x.first, 0.0},
        {range_x.second, 0.0},
    });
    // x = 0 — ось ординат (h / Im / |W| / φ)
    ver->replace(QList<QPointF>{
        {0.0, range_y.first},
        {0.0, range_y.second},
    });

    attach_to_axes(chart, hor);
    attach_to_axes(chart, ver);
    hideLegendMarker(chart, hor);
    hideLegendMarker(chart, ver);
}

void createChartContextMenu(QChartView* chart_view) {
    auto* save_image_action = new QAction(CHART_TR("Сохранить как PNG"), chart_view);
    auto* save_text_action  = new QAction(CHART_TR("Сохранить как TXT"), chart_view);
    auto* copy_action       = new QAction(CHART_TR("Копировать изображение"), chart_view);
    auto* properties_action = new QAction(CHART_TR("Свойства"), chart_view);

    QChart* chart = chart_view->chart();

    QObject::connect(save_image_action, &QAction::triggered, [chart, chart_view] {
        const QString file_name =
            QFileDialog::getSaveFileName(nullptr, CHART_TR("Сохранить график"), chart->title(),
                                         CHART_TR("Рисунок PNG (*.png);;Все файлы (*)"));
        if (!file_name.isEmpty() && !chart_view->grab().save(file_name, "png"))
            QMessageBox::warning(nullptr, CHART_TR("Ошибка"), CHART_TR("Не удалось сохранить график!"));
    });

    QObject::connect(save_text_action, &QAction::triggered, [chart] {
        const QString file_name =
            QFileDialog::getSaveFileName(nullptr, CHART_TR("Сохранить график"), chart->title(),
                                         CHART_TR("Текст txt (*.txt);;Все файлы (*)"));
        if (!file_name.isEmpty() && !saveChartToFile(file_name, chart))
            QMessageBox::warning(nullptr, CHART_TR("Ошибка"), CHART_TR("Не удалось сохранить график!"));
    });

    QObject::connect(copy_action, &QAction::triggered, [chart_view] {
        QApplication::clipboard()->setImage(chart_view->grab().toImage());
    });

    QObject::connect(properties_action, &QAction::triggered, [chart] {
        ChartDialog dialog(chart);
        dialog.exec();
    });

    auto* context_menu = new QMenu(chart_view);
    context_menu->addAction(save_image_action);
    context_menu->addAction(save_text_action);
    context_menu->addAction(copy_action);
    context_menu->addSeparator();
    context_menu->addAction(properties_action);

    chart_view->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(chart_view, &QChartView::customContextMenuRequested,
                     [context_menu, chart_view](const QPoint& pos) {
                         context_menu->exec(chart_view->mapToGlobal(pos));
                     });
}

void removeAllSeries(QChart* chart) {
    const auto all = chart->series();
    for (auto* series : all) {
        if (isGuideSeries(series->name()))
            continue;
        chart->removeSeries(series);
        delete series;
    }
    chart->update();
}

void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y) {
    auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (!axis_x || !axis_y)
        return;
    axis_x->setRange(range_x.first, range_x.second);
    axis_y->setRange(range_y.first, range_y.second);

    updateOriginGuides(chart, range_x, range_y);

    for (auto* series : chart->series()) {
        if (!series->attachedAxes().contains(axis_x))
            series->attachAxis(axis_x);
        if (!series->attachedAxes().contains(axis_y))
            series->attachAxis(axis_y);
    }
}

void addRealSeries(QChart* chart, const VecPair& points, const QString& title, std::size_t index) {
    if (points.empty())
        return;
    auto* series = new QLineSeries;
    series->setName(title);
    series->setPen(penForIndex(index));
    series->replace(toPoints(points));
    chart->addSeries(series);
}

void addComplexSeries(QChart* chart, const VecComp& points, const QString& title, std::size_t index) {
    if (points.empty())
        return;
    auto* series = new QLineSeries;
    series->setName(title);
    series->setPen(penForIndex(index));
    series->replace(toPoints(points));
    chart->addSeries(series);
}

bool replaceLastRealSeries(QChart* chart, const VecPair& points, const QString& title) {
    auto* series = lastDataSeries(chart);
    if (!series || points.empty())
        return false;
    series->setName(title);
    series->replace(toPoints(points));
    return true;
}

bool replaceLastComplexSeries(QChart* chart, const VecComp& points, const QString& title) {
    auto* series = lastDataSeries(chart);
    if (!series || points.empty())
        return false;
    series->setName(title);
    series->replace(toPoints(points));
    return true;
}

bool saveChartToFile(const QString& fileName, QChart* chart) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    for (QAbstractSeries* series : chart->series()) {
        if (isGuideSeries(series->name()))
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

QChartView* makeChartView(QChart* chart, QWidget* parent, const QString& title, const QString& titleX,
                          const QString& titleY) {
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::NoAnimation);
    createAxes(chart, titleX, titleY);

    const Pair def_x{-1.0, 1.0};
    const Pair def_y{-1.0, 1.0};
    if (auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr)))
        axis_x->setRange(def_x.first, def_x.second);
    if (auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr)))
        axis_y->setRange(def_y.first, def_y.second);
    // Guides first → data series drawn later sit on top
    updateOriginGuides(chart, def_x, def_y);

    auto* view = new QChartView(chart, parent);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    createChartContextMenu(view);
    return view;
}

} // namespace chart_utils
