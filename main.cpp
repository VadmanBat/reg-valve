#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCharts>

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

            // После загрузки данных, создайте серию и добавьте ее в график
            double xMin(-10), xMax(10);
            double yMin(-10), yMax(10);

            QLineSeries *series = new QLineSeries();
            for (const QPointF &point : points) {
                series->append(point);
            }
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).first()->setRange(xMin, xMax);
            chart->axes(Qt::Vertical).first()->setRange(yMin, yMax);
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
