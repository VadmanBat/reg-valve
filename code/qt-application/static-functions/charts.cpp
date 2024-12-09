//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"
#include "../structures/chart-properties-dialog.hpp"

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
    auto style = QApplication::style();

    QIcon saveIcon = style->standardIcon(QStyle::SP_DialogSaveButton);
    QIcon copyIcon = style->standardIcon(QStyle::QStyle::SP_FileIcon);
    QIcon settingsIcon = style->standardIcon(QStyle::SP_DialogApplyButton);

    auto saveImageAction = new QAction(saveIcon, tr("Сохранить как PNG"), chartView);
    auto saveTextAction = new QAction(saveIcon, tr("Сохранить как TXT"), chartView);
    auto copyAction = new QAction(copyIcon, tr("Копировать в буфер обмена"), chartView);
    auto propertiesAction = new QAction(settingsIcon, tr("Свойства"), chartView);

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
        ChartPropertiesDialog dialog(chart);
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

bool Application::saveChartToFile(const QString& fileName, QChart* chart) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    QList <QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries* series : seriesList) {
        out << "Name: " << series->name() << "\n";
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

/*
std::vector <Pair> Application::readVectorFromFile(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    QList <QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries* series : seriesList) {
        out << "Name: " << series->name() << "\n";
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
}*/