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

#include <iostream>

class ChartDialog : public QDialog {
Q_OBJECT

private:
    void applyStyles() {
        QString commonStyle = R"(
        QWidget {
            font-family: "Helvetica Neue", sans-serif;
            font-size: 12px;
            color: #333333;
            background-color: #f5f5f5;
        }

        QLineEdit {
            padding: 6px 8px;
            border: 1px solid #c0c0c0;
            border-radius: 4px;
            background-color: white;
            color: #333333;
            selection-background-color: #e0eaf7;
            selection-color: black;
        }

        QLineEdit:hover {
            border-color: #909090;
        }

        QLineEdit:focus {
            border-color: #4682B4;
            outline: 1px solid rgba(70, 130, 180, 0.5);
        }

        QComboBox {
            padding: 6px 8px;
            border: 1px solid #c0c0c0;
            border-radius: 4px;
            background-color: white;
            color: #333333;
            selection-background-color: #e0eaf7;
             selection-color: black;
        }

        QComboBox:hover {
            border-color: #909090;
        }

        QComboBox:focus {
            border-color: #4682B4;
            outline: 1px solid rgba(70, 130, 180, 0.5);
        }

        QComboBox::drop-down {
          border: 0px;
        }

        QComboBox::down-arrow {
            image: url(./down_arrow.png); /*заменить на свой путь*/
        }

        QComboBox QAbstractItemView {
          border: 1px solid #c0c0c0;
          selection-background-color: #e0eaf7;
          selection-color: black;
          outline: none;
        }

        QSpinBox {
            padding: 6px 8px;
            border: 1px solid #c0c0c0;
            border-radius: 4px;
            background-color: white;
            color: #333333;
            selection-background-color: #e0eaf7;
            selection-color: black;
        }

        QSpinBox:hover {
             border-color: #909090;
        }

        QSpinBox:focus {
            border-color: #4682B4;
            outline: 1px solid rgba(70, 130, 180, 0.5);
        }

       /* Кнопки (QPushButton) */
         QPushButton[text="Применить"], QPushButton[text="Отменить"] {
            padding: 8px 16px;
            border-radius: 4px;
            color: white;
        }

        /* Кнопка "Применить" */
        QPushButton[text="Применить"] {
            background-color: #4CAF50; /* Зеленоватый цвет */
             border: 1px solid #4CAF50;
        }

        QPushButton[text="Применить"]:hover {
            background-color: #45a049; /* Затемненный оттенок зеленого при наведении */
            border: 1px solid #45a049;
        }

        QPushButton[text="Применить"]:pressed {
           background-color: #367c39;
           border: 1px solid #367c39;
        }

        /* Кнопка "Отменить" */
        QPushButton[text="Отменить"] {
            background-color: #F44336; /* Красноватый цвет */
            border: 1px solid #F44336;
        }

        QPushButton[text="Отменить"]:hover {
            background-color: #da190b; /* Затемненный оттенок красного при наведении */
             border: 1px solid #da190b;
        }

          QPushButton[text="Отменить"]:pressed {
           background-color: #b6140a;
           border: 1px solid #b6140a;
        }

        QPushButton[style*="background-color:"] {
             border: 1px solid #c0c0c0;
             min-width: 30px;
             min-height: 30px;
         }


        QLabel {
            font-size: 12px;
            padding: 5px;
        }
    )";
        setStyleSheet(commonStyle);
    }

public:
    explicit ChartDialog(QChart* chartPtr, QWidget* parent = nullptr) :
            QDialog(parent), chart(chartPtr), series_size(chart->series().size())
    {
        setWindowTitle(tr("Свойства графика"));
        setWindowIcon(QIcon::fromTheme("dialog-information"));
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        titleEdit       = new QLineEdit(chart->title());
        xAxisLabelEdit  = new QLineEdit;
        yAxisLabelEdit  = new QLineEdit;

        if (auto axis = getAxis(Qt::Horizontal))
            xAxisLabelEdit->setText(axis->titleText());
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
        //applyStyles();
    }

private:
    QLineEdit* titleEdit;
    QLineEdit* xAxisLabelEdit;
    QLineEdit* yAxisLabelEdit;
    QChart* chart;

    qsizetype series_size;
    QVector <QPen> initPens, currentPens;

    QVector <QLineEdit*> seriesNameEdits;
    QVector <QPushButton*> colorButtons;
    QVector <QSpinBox*> widthSpinBoxes;
    QVector <QComboBox*> styleComboBoxes;
    QVector <QLineSeries*> lineSeriesPointers;

    static QLabel* createLabel(const QString& text, Qt::AlignmentFlag flag) {
        auto label = new QLabel(text);
        label->setAlignment(flag);
        label->setStyleSheet("padding: 5px;");
        return label;
    };

    [[nodiscard]] QAbstractAxis* getAxis(Qt::Orientation orientation) const {
        const auto axes = chart->axes(orientation);
        return axes.isEmpty() ? nullptr : axes.first();
    }

    void changeSeriesColor(int index) {
        QColor newColor = QColorDialog::getColor(lineSeriesPointers[index]->pen().color(), this);
        if (newColor.isValid()) {
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

        layout->addWidget(createLabel("Название", Qt::AlignCenter), 0, 0);
        layout->addWidget(createLabel("Цвет", Qt::AlignCenter), 0, 1);
        layout->addWidget(createLabel("Толщина", Qt::AlignCenter), 0, 2);
        layout->addWidget(createLabel("Стиль", Qt::AlignCenter), 0, 3);

        int index = 0;
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

            connect(colorButton, &QPushButton::clicked, [this, index]() { changeSeriesColor(index); });
            connect(widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, index]() { changeSeriesStyle(index); });
            connect(styleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, index](){ changeSeriesStyle(index); });
            ++index;
            layout->addWidget(nameEdit, index, 0);
            layout->addWidget(colorButton, index, 1);
            layout->addWidget(widthSpinBox, index, 2);
            layout->addWidget(styleComboBox, index, 3);

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
        for (qsizetype i = 0; i < series_size; ++i)
            lineSeriesPointers[i]->setPen(initPens[i]);
        chart->update();
        accept();
    }
};

#endif //REGVALVE_CHART_DIALOG_HPP