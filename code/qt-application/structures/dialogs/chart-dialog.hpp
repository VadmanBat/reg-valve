//
// Created by Vadma on 07.12.2024.
//

#ifndef REGVALVE_CHART_DIALOG_HPP
#define REGVALVE_CHART_DIALOG_HPP

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QFont>
#include <QtCharts>

class ChartDialog : public QDialog {
Q_OBJECT

public:
    explicit ChartDialog(QChart* chartPtr, QWidget* parent = nullptr) :
            QDialog(parent), chart(chartPtr), series_size(chart->series().size())
    {
        setWindowTitle(tr("Свойства графика"));
        setWindowIcon(QIcon::fromTheme("dialog-information"));
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        titleEdit       = new QLineEdit(chart->title());
        xAxisLabelEdit  = new QLineEdit;
        if (auto axis = getAxis(Qt::Horizontal))
            xAxisLabelEdit->setText(axis->titleText());
        yAxisLabelEdit = new QLineEdit;
        if (auto axis = getAxis(Qt::Vertical))
            yAxisLabelEdit->setText(axis->titleText());

        auto applyButton    = new QPushButton(tr("Применить"));
        auto cancelButton   = new QPushButton(tr("Отменить"));

        connect(applyButton, &QPushButton::clicked, this, &ChartDialog::updateChart);
        connect(cancelButton, &QPushButton::clicked, this, &ChartDialog::restoreChart);
        connect(this, &QDialog::rejected, this, &ChartDialog::restoreChart);

        auto buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(applyButton);
        buttonLayout->addWidget(cancelButton);

        auto formLayout = new QFormLayout(this);
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

        formLayout->addRow(tr("Заголовок:"), titleEdit);
        formLayout->addRow(tr("Подпись оси X:"), xAxisLabelEdit);
        formLayout->addRow(tr("Подпись оси Y:"), yAxisLabelEdit);
        if (auto grid = getSeriesGrid(); series_size != 0)
            formLayout->addRow(grid);
        else
            delete grid;
        formLayout->addRow(buttonLayout);

        setLayout(formLayout);
    }

private:
    QLineEdit* titleEdit;
    QLineEdit* xAxisLabelEdit;
    QLineEdit* yAxisLabelEdit;
    QChart* chart;

    int series_size;
    QVector <QPen> initPens, currentPens;

    QVector <QLineEdit*> seriesNameEdits;
    QVector <QPushButton*> colorButtons;
    QVector <QSpinBox*> widthSpinBoxes;
    QVector <QComboBox*> styleComboBoxes;
    QVector <QLineSeries*> lineSeriesPointers;

    [[nodiscard]] QAbstractAxis* getAxis(Qt::Orientation orientation) const {
        const auto axes = chart->axes(orientation);
        return axes.isEmpty() ? nullptr : axes.first();
    }

    void changeSeriesColor(int index) {
        if (auto newColor = QColorDialog::getColor(lineSeriesPointers[index]->pen().color(), this); newColor.isValid()) {
            currentPens[index].setColor(newColor);
            lineSeriesPointers[index]->setPen(currentPens[index]);
            colorButtons[index]->setStyleSheet(QString("background-color: %1").arg(newColor.name()));
        }
    }

    void changeSeriesStyle(int index) {
        currentPens[index].setWidth(widthSpinBoxes[index]->value());
        currentPens[index].setStyle(getTypeLine(styleComboBoxes[index]->currentIndex()));
        lineSeriesPointers[index]->setPen(currentPens[index]);
    }

    QGridLayout* getSeriesGrid() {
        auto layout = new QGridLayout;
        auto series = chart->series();

        auto createCenteredLabel = [](const QString& text) {
            auto label = new QLabel(text);
            label->setAlignment(Qt::AlignCenter);
            return label;
        };

        layout->addWidget(createCenteredLabel("Название"), 0, 0);
        layout->addWidget(createCenteredLabel("Цвет"), 0, 1);
        layout->addWidget(createCenteredLabel("Толщина"), 0, 2);
        layout->addWidget(createCenteredLabel("Стиль"), 0, 3);

        for (int i = 0; i < series_size; ++i) {
            auto lineSeries = qobject_cast<QLineSeries*>(series[i]);
            if (!lineSeries || lineSeries->name() == "hor-line" || lineSeries->name() == "ver-line")
                continue;

            auto nameEdit       = new QLineEdit(lineSeries->name());
            auto colorButton    = new QPushButton;
            auto widthSpinBox   = new QSpinBox;
            auto styleComboBox  = new QComboBox;

            colorButton->setStyleSheet(QString("background-color: %1").arg(lineSeries->pen().color().name()));

            widthSpinBox->setMinimum(1);
            widthSpinBox->setMaximum(10);
            widthSpinBox->setValue(lineSeries->pen().width());

            styleComboBox->addItem("Сплошная");
            styleComboBox->addItem("Штриховая");
            styleComboBox->addItem("Точечная");
            styleComboBox->addItem("Штрих-точка");
            styleComboBox->addItem("Штрих-точка-точка");
            styleComboBox->setCurrentIndex(getIndexLine(lineSeries->pen().style()));

            connect(colorButton, &QPushButton::clicked, [this, i]() { changeSeriesColor(i); });
            connect(widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, i]() { changeSeriesStyle(i); });
            connect(styleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, i](){ changeSeriesStyle(i); });

            layout->addWidget(nameEdit, i + 1, 0);
            layout->addWidget(colorButton, i + 1, 1);
            layout->addWidget(widthSpinBox, i + 1, 2);
            layout->addWidget(styleComboBox, i + 1, 3);

            seriesNameEdits.append(nameEdit);
            colorButtons.append(colorButton);
            widthSpinBoxes.append(widthSpinBox);
            styleComboBoxes.append(styleComboBox);
            lineSeriesPointers.append(lineSeries);
            initPens.append(lineSeries->pen());
        }
        series_size = lineSeriesPointers.size();
        currentPens = initPens;
        return layout;
    }

    static Qt::PenStyle getTypeLine(int index) {
        switch (index) {
            case 0:
                return Qt::SolidLine;
            case 1:
                return Qt::DashLine;
            case 2:
                return Qt::DotLine;
            case 3:
                return Qt::DashDotLine;
            case 4:
                return Qt::DashDotDotLine;
            default:
                return Qt::NoPen;
        }
    }

    static int getIndexLine(Qt::PenStyle style) {
        switch (style) {
            case Qt::SolidLine:
                return 0;
            case Qt::DashLine:
                return 1;
            case Qt::DotLine:
                return 2;
            case Qt::DashDotLine:
                return 3;
            case Qt::DashDotDotLine:
                return 4;
            default:
                return -1;
        }
    }

private slots:
    void updateChart() {
        chart->setTitle(titleEdit->text());
        if (auto axis = chart->axes(Qt::Horizontal).first())
            axis->setTitleText(xAxisLabelEdit->text());
        if (auto axis = chart->axes(Qt::Vertical).first())
            axis->setTitleText(yAxisLabelEdit->text());
        for (int i = 0; i < series_size; ++i)
            lineSeriesPointers[i]->setName(seriesNameEdits[i]->text());
        chart->update();
        accept();
    }

    void restoreChart() {
        for (int i = 0; i < series_size; ++i)
            lineSeriesPointers[i]->setPen(initPens[i]);
        chart->update();
        accept();
    }
};

#endif //REGVALVE_CHART_DIALOG_HPP