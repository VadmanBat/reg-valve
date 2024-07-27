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

#include "code/qt-application/structures/line-edit.hpp"
#include "code/qt-application/structures/reg-parameter.h"

#include "../transfer-function/transfer-function.hpp"

using namespace QtCharts;

class Application : public QWidget {
private:
    using ChartData = std::tuple <QChart*, QString, QString, QString>;
    using ChartsDataset = std::vector <ChartData>;

    static void showError(const QString& errorMessage);
    /// update css-field:
    template <typename T>
    static void updateStyleSheetProperty(T* widget, const QString& property, const QString& value) {
        static_assert(std::is_base_of<QWidget, T>::value, "T must be a QWidget or derived class");

        QString style = widget->styleSheet();
        QString replacement = QString("%1: %2;").arg(property,value);
        if (style.contains(property)) {
            QString pattern = QString("%1: \\w+;").arg(property);
            style = style.replace(QRegExp(pattern), replacement);
        } else {
            style += replacement;
        }
        widget->setStyleSheet(style);
    }
    template <typename T>
    static void updateStyleSheetProperty(T* widget, const QString& selector, const QString& property, const QString& value) {
        static_assert(std::is_base_of<QWidget, T>::value, "T must be a QWidget or derived class");

        QString style = widget->styleSheet();
        QString replacement = QString("%1: %2;").arg(property, value);
        QString pattern = QString("(%1\\s*\\{[^}]*)(%2\\s*:\\s*[^;]+;)([^}]*\\})").arg(selector, property);

        QRegExp regex(pattern);
        if (regex.indexIn(style) != -1) {
            style.replace(regex, QString("\\1%1\\3").arg(replacement));
        } else {
            QString selectorPattern = QString("(%1\\s*\\{[^}]*)(\\})").arg(selector);
            QRegExp selectorRegex(selectorPattern);
            if (selectorRegex.indexIn(style) != -1) {
                style.replace(selectorRegex, QString("\\1 %2 \\2").arg(replacement));
            } else {
                style += QString("%1 { %2 }").arg(selector, replacement);
            }
        }

        widget->setStyleSheet(style);
    }

    static QString getColor(const double& value);
    static double getValue(QString text);
    static void adjustLineEditWidth(QLineEdit* lineEdit);

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
    static QString correctLine(const QString& text);
    static QLayout* createLineEdit(const QString& title, std::vector <LineEdit*>& lineEdits, QDoubleValidator* validator);
    static QLayout* createTransferFunctionForm(std::vector <LineEdit*>& numerator, std::vector <LineEdit*>& denominator,
                                               std::size_t n = 6, std::size_t m = 6, const QString& title = "W(p) = ");

    /// charts functions:
    static QLayout* createCharts(const ChartsDataset& charts, QWidget* tab);
    static void createAxes(QChart* chart, const QString &titleX, const QString &titleY);
    static void eraseLastSeries(QChart* chart);
    static void removeAllSeries(QChart* chart);
    static void updateAxes(QChart* chart);

    template <class Points>
    static void addPoints(QChart* chart, const Points& points, const QString& title) {
        auto series = new QLineSeries();
        series->setName(title);
        for (const auto& [x, y] : points)
            series->append(x, y);
        chart->addSeries(series);
    }
    template <class Points>
    static void addComplexPoints(QChart* chart, const Points& points, const QString& title) {
        auto series = new QLineSeries();
        series->setName(title);
        for (const auto& point : points)
            series->append(point.real(), point.imag());
        chart->addSeries(series);
    }
    template <class Point>
    static void appendPoint(QChart* chart, const Point& point, int index = 0) {
        const auto& [x, y](point);
        dynamic_cast<QScatterSeries*>(chart->series().at(index))->append(x, y);
    }
    template <class Point>
    static void appendComplexPoint(QChart* chart, const Point& point, int index = 0) {
        dynamic_cast<QScatterSeries*>(chart->series().at(index))->append(point.real(), point.imag());
    }

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

        applyStyles();
    }

private:
    static bool numIsValidInput(const MathCore::Vec &num, const MathCore::Vec &den);

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

    std::vector <LineEdit*> numNumerator, numDenominator;
    std::vector <LineEdit*> regNumerator, regDenominator;

    std::vector <RegParameter*> regParameters;

    void applyStyles();
};

#endif //REGVALVE_APPLICATION_H
