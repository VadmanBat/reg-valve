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

#include "../reg.core.hpp"
#include "../transfer-function/transfer-function.hpp"

using namespace QtCharts;

class Application : public QWidget {
private:
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

    static MathCore::Vec getLineEditData(QHBoxLayout *layout);
    static void updateStyleSheetProperty(QLineEdit *lineEdit, const QString &property, const QString &value);
    static QString correctLine(const QString &text);
    static void createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator);

    /// charts-funcs:
    static QHBoxLayout* createCharts(std::vector <std::tuple <QChart*, QString, QString, QString>> charts, QWidget* tab);
    static void createAxes(QChart *chart, const QString &titleX, const QString &titleY);
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
    static DoubleSlider* createParameterForm(const char *name, QHBoxLayout *layout, double lower, double upper, double min, double max);
    static std::vector <DoubleSlider*> createControllerParameterForms(QVBoxLayout *layout);

    QWidget* createExpTab();
    QWidget* createNumTab();
    QWidget* createRegTab();

public:
    Application(QWidget *parent = 0) : QWidget(parent) {
        QTabWidget *tabWidget = new QTabWidget(this);
        // Добавление вкладок в QTabWidget
        tabWidget->addTab(createExpTab(), "КЧХ по h(t)");
        tabWidget->addTab(createNumTab(), "КЧХ по W(p)");
        tabWidget->addTab(createRegTab(), "Ручная настройка регулятора");

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(tabWidget);
        setLayout(mainLayout);
    }

private slots:
    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Файлы данных (*.txt *.csv)");
        if (!fileName.isEmpty()) {
            // Здесь должен быть ваш код для чтения файла и построения данных
            // Например, вы можете прочитать файл и заполнить QVector<QPointF> points данными для построения графика
            // ...

        }
    }

private:
    QChart *expChartTranResp{new QChart}, *expChartFreqResp{new QChart};
    QChart *numChartTranResp{new QChart}, *numChartFreqResp{new QChart};
    QChart *regChartTranResp{new QChart}, *regChartFreqResp{new QChart};

    const std::vector <std::tuple <QChart*, QString, QString, QString>> EXP_CHARTS = {
            {expChartTranResp, "Переходная характеристика", "Время t, секунды", "h(t)"},
            {expChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
    const std::vector <std::tuple <QChart*, QString, QString, QString>> NUM_CHARTS = {
            {numChartTranResp, "Переходная характеристика", "Время t, секунды", "Параметр h(t), [ед.изм.] / %ХРО"},
            {numChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
    const std::vector <std::tuple <QChart*, QString, QString, QString>> REG_CHARTS = {
            {regChartTranResp, "Переходная характеристика", "Время t, секунды", "h(t)"},
            {regChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
};

#include <QLineEdit>
#include <QFocusEvent>

class MyLineEdit : public QLineEdit {
public:
    explicit MyLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

protected:
    void focusOutEvent(QFocusEvent *event) override {
        QLineEdit::focusOutEvent(event);
        emit editingFinished(); // Явно вызываем сигнал editingFinished
    }
};

#endif //REGVALVE_APPLICATION_H
