#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCharts>

#include "code/math-core.h"
#include "code/convert-core.h"

using namespace QtCharts;

class GraphWindow : public QWidget {
public:
    GraphWindow(QWidget *parent = 0) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        QPushButton *openFileButton = new QPushButton("Открыть файл", this);
        connect(openFileButton, &QPushButton::clicked, this, &GraphWindow::openFile);
        layout->addWidget(openFileButton);

        chart = new QChart();
        chartView = new QChartView(chart, this);
        layout->addWidget(chartView);

        setLayout(layout);
    }

private slots:
    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Файлы данных (*.txt *.csv)");
        if (!fileName.isEmpty()) {
            // Здесь должен быть ваш код для чтения файла и построения данных
            // Например, вы можете прочитать файл и заполнить QVector<QPointF> points данными для построения графика
            // ...


            QLineSeries *series = new QLineSeries();
            series->setName("КЧХ выбранного сигнала");
            for (const QPointF &point : points) {
                series->append(point);
            }

            chart->setTitle("Комплексно-частотная характеристика (КЧХ)");
            chart->addSeries(series);
            chart->createDefaultAxes();

            QAbstractAxis *axisX = chart->axes(Qt::Horizontal).first();
            QAbstractAxis *axisY = chart->axes(Qt::Vertical).first();

            if (QValueAxis *axisXValue = qobject_cast<QValueAxis *>(axisX); axisXValue) {
                axisXValue->setTitleText("Реальная ось");
                axisXValue->setLabelFormat("%.2f"); // Формат подписи оси X
            }

            if (QValueAxis *axisYValue = qobject_cast<QValueAxis *>(axisY); axisYValue) {
                axisYValue->setTitleText("Мнимая ось");
                axisYValue->setLabelFormat("%.2f"); // Формат подписи оси Y
            }
        }
    }

private:
    QChart *chart;
    QChartView *chartView;
    QVector <QPointF> points; // Данные для графика
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    GraphWindow window;
    window.show();
    return app.exec();
}
