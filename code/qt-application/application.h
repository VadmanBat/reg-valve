//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_APPLICATION_H
#define REGVALVE_APPLICATION_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCharts>

#include "double-slider.hpp"
#include "line-edit.hpp"

#include "../transfer-function/transfer-function.hpp"

using namespace QtCharts;

class Application : public QWidget {
private:
    using ChartData = std::tuple <QChart*, QString, QString, QString>;
    using ChartsDataset = std::vector <ChartData>;

    using SliderData = std::tuple <QString, double, double, double, double>;
    using SlidersDataset = std::vector <SliderData>;

    static QString getColor(const double& value);
    static double getValue(QString text);
    static void adjustLineEditWidth(QLineEdit *lineEdit);

    template <class Container>
    static Container reverseOptimize(const Container& container) {
        auto first(container.rbegin());
        const auto last(container.rend());
        if (first == last)
            return Container();
        while (first != last && *first == 0)
            ++first;
        return Container(first, last);
    }

    static MathCore::Vec getLineEditData(const std::vector <LineEdit*>& lineEdits);
    static void updateStyleSheetProperty(QLineEdit *lineEdit, const QString &property, const QString &value);
    static QString correctLine(const QString &text);
    static QLayout* createLineEdit(const QString& name, std::vector <LineEdit*>& lineEdits, QDoubleValidator* validator);
    static QLayout* createTransferFunctionForm(std::vector <LineEdit*>& numerator, std::vector <LineEdit*>& denominator,
                                               std::size_t n = 6, std::size_t m = 6, const QString& title = "W(p) = ");

    /// charts-funcs:
    static QLayout* createCharts(ChartsDataset charts, QWidget* tab);
    static void createAxes(QChart *chart, const QString &titleX, const QString &titleY);
    static void eraseLastSeries(QChart *chart);
    static void removeAllSeries(QChart *chart);
    static void updateAxes(QChart *chart);

    template <class Points>
    void addPoints(QChart* chart, const Points& points, const QString& title) {
        QLineSeries *series = new QLineSeries();
        series->setName(title);
        for (const auto& [x, y] : points)
            series->append(x, y);
        chart->addSeries(series);
    }
    template <class Points>
    void addComplexPoints(QChart* chart, const Points& points, const QString& title) {
        QLineSeries *series = new QLineSeries();
        series->setName(title);
        for (const auto& point : points)
            series->append(point.real(), point.imag());
        chart->addSeries(series);
    }
    template <class Point>
    void appendPoint(QChart *chart, const Point& point, int index = 0) {
        const auto& [x, y](point);
        static_cast<QScatterSeries*>(chart->series().at(index))->append(x, y);
    }
    template <class Point>
    void appendComplexPoint(QChart *chart, const Point& point, int index = 0) {
        static_cast<QScatterSeries*>(chart->series().at(index))->append(point.real(), point.imag());
    }

    static void setSpinBox(QDoubleSpinBox *spinBox, double min, double max, double value, const char *prefix);
    static void updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, DoubleSlider *slider);
    static std::pair <DoubleSlider*, QLayout*> createSliderForm(const SliderData& data);
    static QLayout* createSlidersForm(std::vector <DoubleSlider*>& sliders, const SlidersDataset& data);

    QWidget* createExpTab();
    QWidget* createNumTab();
    QWidget* createRegTab();

public:
    explicit Application(QWidget* parent = nullptr) : QWidget(parent) {
        auto tabWidget = new QTabWidget(this);
        tabWidget->addTab(createExpTab(), "КЧХ по h(t)");
        tabWidget->addTab(createNumTab(), "КЧХ по W(p)");
        tabWidget->addTab(createRegTab(), "Ручная настройка регулятора");

        auto mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(tabWidget);
        setLayout(mainLayout);
    }

private slots:
    void expOpenFile();

    void numAddTransferFunction();
    void numReplaceTransferFunction();
    void numClearCharts();

    void regAddTransferFunction();
    void regReplaceTransferFunction();
    void regClearCharts();

private:
    QChart *expChartTranResp{new QChart}, *expChartFreqResp{new QChart};
    QChart *numChartTranResp{new QChart}, *numChartFreqResp{new QChart};
    QChart *regChartTranResp{new QChart}, *regChartFreqResp{new QChart};

    const ChartsDataset EXP_CHARTS = {
            {expChartTranResp, "Переходная характеристика", "Время t, секунды", "h(t)"},
            {expChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
    const ChartsDataset NUM_CHARTS = {
            {numChartTranResp, "Переходная характеристика", "Время t, секунды", "Параметр h(t), [ед.изм.] / %ХРО"},
            {numChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
    const ChartsDataset REG_CHARTS = {
            {regChartTranResp, "Переходная характеристика", "Время t, секунды", "h(t)"},
            {regChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };

    const SlidersDataset REG_SLIDERS = {
            {"K<sub>p</sub>", 0.1, 50, 1, 10},
            {"T<sub>u</sub>", 0.1, 2000, 1, 120},
            {"T<sub>d</sub>", 0.1, 2000, 1, 60}
    };

    std::vector <LineEdit*> numNumerator, numDenominator;
    std::vector <LineEdit*> regNumerator, regDenominator;

    std::vector <DoubleSlider*> regSliders;
};

#endif //REGVALVE_APPLICATION_H
