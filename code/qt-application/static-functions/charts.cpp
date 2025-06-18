//
// Created by Vadim on 13.07.2024.
//
#include "../application.h"
#include "code/structures/dialogs/chart-dialog.hpp"
#include "code/structures/dialogs/mod-par-dialog.hpp"

QLayout* Application::createCharts(const ChartsDataset& charts, QWidget* tab) {
    auto layout = new QHBoxLayout;
    for (auto& [chart, title, titleX, titleY] : charts) {
        chart->setTitle(title);
        createAxes(chart, titleX, titleY);
        auto chartView = new QChartView(chart, tab);
        chartView->setRenderHint(QPainter::Antialiasing);
        createChartContextMenu(chartView);
        layout->addWidget(chartView);
    }
    return layout;
}

void Application::createChartContextMenu(QChartView* chartView) {
    auto saveImageAction    = new QAction(QIcon::fromTheme("document-save-as"), tr("Сохранить как PNG"), chartView);
    auto saveTextAction     = new QAction(QIcon::fromTheme("document-save-as"), tr("Сохранить как TXT"), chartView);
    auto copyAction         = new QAction(QIcon::fromTheme("edit-copy"), tr("Копировать изображение"), chartView);
    auto propertiesAction   = new QAction(QIcon::fromTheme("document-properties"), tr("Свойства"), chartView);

    auto chart = chartView->chart();

    connect(saveImageAction, &QAction::triggered, [chart, chartView]() {
        QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Сохранить график"), chart->title(), tr("Рисунок в формате PNG (*.png);;Все файлы (*)"));
        if (!fileName.isEmpty()) {
            QPixmap pixmap = chartView->grab();
            if (!pixmap.save(fileName, "png"))
                QMessageBox::warning(nullptr, tr("Ошибка"), tr("Не удалось сохранить график!"));
        }
    });

    connect(saveTextAction, &QAction::triggered, [chart]() {
        QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Сохранить график"), chart->title(), tr("Текст в формате txt (*.txt);;Все файлы (*)"));
        if (!fileName.isEmpty()) {
            if (!saveChartToFile(fileName, chart))
                QMessageBox::warning(nullptr, tr("Ошибка"), tr("Не удалось сохранить график!"));
        }
    });

    connect(copyAction, &QAction::triggered, [chartView]() {
        QPixmap pixmap = chartView->grab();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(pixmap.toImage());
    });

    connect(propertiesAction, &QAction::triggered, [chart]() {
        ChartDialog dialog(chart);
        dialog.exec();
    });

    auto contextMenu = new QMenu(chartView);

    contextMenu->addAction(saveImageAction);
    contextMenu->addAction(saveTextAction);
    contextMenu->addAction(copyAction);
    contextMenu->addSeparator();
    contextMenu->addAction(propertiesAction);

    chartView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chartView, &QChartView::customContextMenuRequested, [contextMenu, chartView](const QPoint& pos) {
        contextMenu->exec(chartView->mapToGlobal(pos));
    });
}

void Application::eraseLastSeries(QChart* chart) {
    if (chart->series().empty())
        return;

    auto series = chart->series().back();
    chart->removeSeries(series);
    delete series;
    chart->update();
}

void Application::removeAllSeries(QChart* chart) {
    for (auto series : chart->series()) {
        const auto name = series->name();
        if (name == "hor-line" || name == "ver-line")
            continue;
        chart->removeSeries(series);
        delete series;
    }
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
    return {
        std::abs(min) < 1e-3 * range ? 0 : min - 0.05 * range,
        std::abs(max) < 1e-3 * range ? 0 : max + 0.05 * range
    };

}

void Application::updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y) {
    auto axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    auto axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());

    axisX->setRange(range_x.first, range_x.second);
    axisY->setRange(range_y.first, range_y.second);

    for (auto series : chart->series()) {
        if (!series->attachedAxes().contains(axisX))
            series->attachAxis(axisX);
        if (!series->attachedAxes().contains(axisY))
            series->attachAxis(axisY);
    }
}

void Application::addHorLine(QChart* chart, qreal value, const QPen& pen) {
    auto axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    auto axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

    if (!axisX || !axisY) return;

    auto line = new QLineSeries;
    line->setName("hor-line");
    line->setPen(pen);
    chart->addSeries(line);
    line->attachAxis(axisX);
    line->attachAxis(axisY);
    chart->legend()->markers(line).first()->setVisible(false);

    QObject::connect(axisX, &QValueAxis::rangeChanged, [line, value](qreal min, qreal max) {
        line->clear();
        line->append(min, value);
        line->append(max, value);
    });

    line->append(axisX->min(), value);
    line->append(axisX->max(), value);
}

void Application::addVerLine(QChart* chart, qreal value, const QPen& pen) {
    auto axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    auto axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

    if (!axisX || !axisY) return;

    auto line = new QLineSeries;
    line->setName("ver-line");
    line->setPen(pen);
    chart->addSeries(line);
    line->attachAxis(axisX);
    line->attachAxis(axisY);
    chart->legend()->markers(line).first()->setVisible(false);

    QObject::connect(axisY, &QValueAxis::rangeChanged, [line, value](qreal min, qreal max) {
        line->clear();
        line->append(value, min);
        line->append(value, max);
    });

    line->append(value, axisY->min());
    line->append(value, axisY->max());
}

bool Application::saveChartToFile(const QString& fileName, QChart* chart) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    QList <QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries* series : seriesList) {
        const auto name = series->name();
        if (name == "hor-line" || name == "ver-line")
            continue;
        out << "Name: " << name << "\n";
        if (auto xySeries = qobject_cast<QXYSeries*>(series)) {
            for (int i = 0; i < xySeries->count(); ++i)
                out << xySeries->at(i).x() << ", " << xySeries->at(i).y() << "\n";
        } else if (auto lineSeries = qobject_cast<QLineSeries*>(series)) {
            for (int i = 0; i < lineSeries->count(); ++i)
                out << lineSeries->at(i).x() << ", " << lineSeries->at(i).y() << "\n";
        } else if (auto scatterSeries = qobject_cast<QScatterSeries*>(series)) {
            for (int i = 0; i < scatterSeries->count(); ++i)
                out << scatterSeries->at(i).x() << ", " << scatterSeries->at(i).y() << "\n";
        } else if (auto pieSeries = qobject_cast<QPieSeries*>(series)) {
            for (QPieSlice* slice : pieSeries->slices())
                out << slice->label() << ", " << slice->percentage() << "\n";
        } else if (auto barSeries = qobject_cast<QBarSeries*>(series)) {
            for (QBarSet* barSet : barSeries->barSets()) {
                out << "Bar Set Name: " << barSet->label() << "\n";
                for (int i = 0; i < barSet->count(); ++i)
                    out << barSet->at(i) << "\n";
            }
        }
        out << "\n";
    }

    file.close();
    return true;
}

#include <fstream>

Application::VecPair Application::readVectorFromFile(const QString& fileName) {
    std::vector <double> numbers;

    std::ifstream file(fileName.toLocal8Bit().toStdString());
    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            std::string clean_word;
            for (char let : word)
                if (std::isdigit(let) || let == '.' || let == '-')
                    clean_word += let;
            if (clean_word.empty())
                continue;
            try {
                numbers.push_back(stod(clean_word));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Ошибка преобразования: " << clean_word << std::endl;
            } catch (const  std::out_of_range& e) {
                std::cerr << "Ошибка переполнения: " << clean_word << std::endl;
            }
        }
    }
    file.close();

    VecPair res;
    const auto n = numbers.size();
    for (std::size_t i = 0; i < n; i += 2)
        res.emplace_back(numbers[i], numbers[i + 1]);

    return res;
}