//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

QString GraphWindow::getColor(const double& value) {
    if (value == 0)
        return "violet";
    return value > 0 ? "blue" : "red";
}

double GraphWindow::getValue(QString text) {
    text.replace(',', '.');
    if (text.count() == 1)
        text += '1';
    return text.toDouble();
}

void GraphWindow::adjustLineEditWidth(QLineEdit* lineEdit) {
    int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 10);
    lineEdit->setMinimumWidth(width);
    lineEdit->setMaximumWidth(width);
}

MathCore::VectorComp GraphWindow::getLineEditData(QHBoxLayout* layout) {
    MathCore::VectorComp data;
    const auto count(layout->count());
    for (int i = 0; i < count; ++i) {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(layout->itemAt(i)->widget());
        if (lineEdit) {
            data.emplace_back(getValue(lineEdit->text()), 0);
        }
    }
    return data;
}

void GraphWindow::updateStyleSheetProperty(QLineEdit* lineEdit, const QString& property, const QString& value) {
    QString style = lineEdit->styleSheet();
    QString replacement = QString("%1: %2;").arg(property).arg(value);
    if (style.contains(property)) {
        QString pattern = QString("%1: \\w+;").arg(property);
        style = style.replace(QRegExp(pattern), replacement);
    }
    else
        style += replacement;
    lineEdit->setStyleSheet(style);
}

void GraphWindow::createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator) {
    layout->addWidget(new QLabel(name));

    int p(-1);
    QLineEdit* lineEdits[6];
    for (auto lineEdit : lineEdits) {
        layout->addWidget(lineEdit = new QLineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText("+0");

        QLabel *label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
        label->setStyleSheet("font-size: 16pt;");
        layout->addWidget(label);

        lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(lineEdit, &QLineEdit::textChanged, [lineEdit] {
            auto text(lineEdit->text());
            if (text.isEmpty()) {
                lineEdit->setText("+0");
                return;
            }
            if (lineEdit->text().at(0).isDigit()) {
                lineEdit->setText('+' + lineEdit->text());
                return;
            }

            adjustLineEditWidth(lineEdit);
            updateStyleSheetProperty(lineEdit, "color", getColor(getValue(text)));
        });

        lineEdit->setMinimumWidth(36);
        lineEdit->setMaximumWidth(36);
        lineEdit->setValidator(validator);
    }
    layout->setAlignment(Qt::AlignLeft);
}

void GraphWindow::setChart(
    QChart* chart,
    const QString& title,
    const QString& axisXTitle,
    const QString& axisYTitle
)
{
    chart->setTitle(title);

    QLineSeries *series = new QLineSeries();
    series->setName("КЧХ выбранного сигнала");
    series->append(0, 0);
    series->append(1, 1);
    series->append(2, 4);
    series->append(3, 9);
    series->append(4, 16);
    series->append(5, 25);
    series->append(6, 36);
    series->append(7, 49);
    series->append(8, 64);
    series->append(9, 81);
    series->append(10, 100);
    chart->addSeries(series);

    chart->createDefaultAxes();

    QAbstractAxis *axisX = chart->axes(Qt::Horizontal).first();
    QAbstractAxis *axisY = chart->axes(Qt::Vertical).first();

    if (QValueAxis *axisXValue = qobject_cast<QValueAxis *>(axisX); axisXValue) {
        axisXValue->setTitleText(axisXTitle);
        axisXValue->setLabelFormat("%.2f"); // Формат подписи оси X
    }
    if (QValueAxis *axisYValue = qobject_cast<QValueAxis *>(axisY); axisYValue) {
        axisYValue->setTitleText(axisYTitle);
        axisYValue->setLabelFormat("%.2f"); // Формат подписи оси Y
    }
}