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

#include "../code/math-core.h"
#include "../code/convert-core.h"

using namespace QtCharts;

class GraphWindow : public QWidget {
private:
    static QString getColor(const double& value);
    static double getValue(QString text);
    static void adjustLineEditWidth(QLineEdit *lineEdit);
    static MathCore::VectorComp getLineEditData(QHBoxLayout *layout);
    static void updateStyleSheetProperty(QLineEdit *lineEdit, const QString &property, const QString &value);
    static void createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator);

    static void setChart(QChart* chart, const QString& title, const QString& axisXTitle, const QString& axisYTitle);

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

    QWidget* createExpTab();
    QWidget* createNumTab();

public:
    GraphWindow(QWidget *parent = 0) : QWidget(parent) {
        QTabWidget *tabWidget = new QTabWidget(this);
        // Добавление вкладок в QTabWidget
        tabWidget->addTab(createExpTab(), "КЧХ по h(t)");
        tabWidget->addTab(createNumTab(), "КЧХ по W(p)");

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
    QChart *expChartH, *expChartFreqResp;
    QChart *numChartH, *numChartFreqResp;
    QChartView *expChartHView, *expChartFreqRespView;
    QChartView *numChartHView, *numChartFreqRespView;
};

#endif //REGVALVE_APPLICATION_H
