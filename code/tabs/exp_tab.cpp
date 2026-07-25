#include "code/tabs/exp_tab.h"
#include "ui_exp_tab.h"

#include "code/charts/chart_utils.hpp"

#include <QFileDialog>
#include <QMessageBox>

ExpTab::ExpTab(QWidget* parent) : QWidget(parent), ui(new Ui::ExpTab) {
    ui->setupUi(this);
    chartTran_ = new QChart;
    chartFreq_ = new QChart;
    ui->chartsLayout->addWidget(chart_utils::makeChartView(
        chartTran_, this, tr("Переходная характеристика"), tr("Время t, секунды"), tr("h(t)")));
    ui->chartsLayout->addWidget(chart_utils::makeChartView(
        chartFreq_, this, tr("Комплексно-частотная характеристика (КЧХ)"), tr("Реальная ось"),
        tr("Мнимая ось")));
    connect(ui->openFileButton, &QPushButton::clicked, this, &ExpTab::openFile);
}

ExpTab::~ExpTab() {
    delete ui;
}

void ExpTab::openFile() {
    const QString fileName =
        QFileDialog::getOpenFileName(this, tr("Открыть файл"), {}, tr("Файлы данных (*.txt *.csv);;Все (*)"));
    if (fileName.isEmpty())
        return;

    auto points = chart_utils::readVectorFromFile(fileName);
    if (points.empty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось прочитать данные из файла."));
        return;
    }

    if (!tranSeries_.empty()) {
        chart_utils::eraseLastSeries(chartTran_);
        tranSeries_.pop_back();
    }

    tranSeries_.push_back(Series(std::move(points)));
    chart_utils::addRealSeries(chartTran_, tranSeries_.back().original(), tr("Эксперимент"), 0, true);
    chart_utils::updateAxes(chartTran_, {tranSeries_.min_x(), tranSeries_.max_x()},
                            chart_utils::computeAxesRange(tranSeries_.min_y(), tranSeries_.max_y()));
}
