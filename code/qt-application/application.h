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

#include "../reg.core.hpp"
#include "../transfer-function/transfer-function.hpp"

using namespace QtCharts;

class GraphWindow : public QWidget {
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

    QWidget* createExpTab();
    QWidget* createNumTab();
    QWidget* createRegTab();

public:
    GraphWindow(QWidget *parent = 0) : QWidget(parent) {
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
    QChart *expChartTranResp, *expChartFreqResp;
    QChart *numChartTranResp, *numChartFreqResp;
    QChart *regChartTranResp, *regChartFreqResp;
    QChartView *expChartTranRespView, *expChartFreqRespView;
    QChartView *numChartTranRespView, *numChartFreqRespView;
    QChartView *regChartTranRespView, *regChartFreqRespView;
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
