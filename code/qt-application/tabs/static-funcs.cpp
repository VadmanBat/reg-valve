//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

void
updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, QSlider *slider);

void
updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, QSlider *slider);

void
updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, QSlider *slider);

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

MathCore::Vec GraphWindow::getLineEditData(QHBoxLayout* layout) {
    MathCore::Vec data;
    data.reserve(6);
    const auto count(layout->count());
    for (int i = 0; i < count; ++i) {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(layout->itemAt(i)->widget());
        if (lineEdit)
            data.emplace_back(getValue(lineEdit->text()));
    }
    return reverseOptimize(data);
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

QString GraphWindow::correctLine(const QString& text) {
    if (text.at(0) == ',')
        return "+0" + text;
    if (text.at(0).isDigit())
        return '+' + text;
    if (text.count() > 1 && text.at(1) == ',')
        return text.at(0) + '0' + text.mid(1);
    return "";
}

void GraphWindow::createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator) {
    layout->addWidget(new QLabel(name));

    int p(-1);
    QLineEdit* lineEdits[6];
    for (auto lineEdit : lineEdits) {
        layout->addWidget(lineEdit = new MyLineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText("+0");

        QLabel *label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
        label->setStyleSheet("font-size: 16pt;");
        layout->addWidget(label);

        lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(lineEdit, &QLineEdit::textChanged, [lineEdit] {
            auto text(lineEdit->text());
            if (auto correctedLine(correctLine(text)); !correctedLine.isEmpty()) {
                lineEdit->setText(correctedLine);
                return;
            }
            adjustLineEditWidth(lineEdit);
            updateStyleSheetProperty(lineEdit, "color", getColor(getValue(text)));
        });
        connect(lineEdit, &QLineEdit::editingFinished, [lineEdit] {
            if (lineEdit->text().isEmpty())
                lineEdit->setText("+0");
        });
        lineEdit->setMinimumWidth(36);
        lineEdit->setMaximumWidth(36);
        lineEdit->setValidator(validator);
    }
    layout->setAlignment(Qt::AlignLeft);
}

void GraphWindow::updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, DoubleSlider *slider) {
    double min = minSpinBox->value();
    double max = maxSpinBox->value();
    int points = pointsSpinBox->value();

    if (min < max && points > 0) {
        slider->setRange(min, max);
        slider->setSingleStep((max - min) / (points - 1));
    }
}

void GraphWindow::setSpinBox(QDoubleSpinBox* spinBox, double min, double max, double value, const char* prefix) {
    spinBox->setRange(min, max);
    spinBox->setDecimals(2);
    spinBox->setValue(value);
    spinBox->setPrefix(prefix);
}

void GraphWindow::createParameterForm(const char* name, QHBoxLayout* layout,
                                      double lower, double upper,
                                      double min, double max)
{
    auto*titleLabel = new QLabel(name);
    titleLabel->setStyleSheet("font-size: 16pt;");
    layout->addWidget(titleLabel);

    auto enableCheckBox = new QCheckBox("Enable");
    layout->addWidget(enableCheckBox);

    auto minSpinBox = new QDoubleSpinBox;
    auto maxSpinBox = new QDoubleSpinBox;
    setSpinBox(minSpinBox, lower, upper, min, "min: ");
    setSpinBox(maxSpinBox, lower, upper, max, "max: ");
    layout->addWidget(minSpinBox);
    layout->addWidget(maxSpinBox);

    auto pointsSpinBox = new QSpinBox;
    pointsSpinBox->setRange(10, 1000);
    pointsSpinBox->setPrefix("Points: ");
    pointsSpinBox->setValue(50);
    layout->addWidget(pointsSpinBox);

    auto slider = new DoubleSlider(Qt::Horizontal);
    layout->addWidget(slider);

    connect(minSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double) {
        updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);
        if (minSpinBox->value() > maxSpinBox->value())
            maxSpinBox->setValue(minSpinBox->value());
    });
    connect(maxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double) {
        updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);
        if (minSpinBox->value() > maxSpinBox->value())
            minSpinBox->setValue(maxSpinBox->value());
    });
    connect(pointsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int) {
        updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);
    });

    connect(slider, &QSlider::valueChanged, [=](int value) {

    });

    updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);

    connect(enableCheckBox, &QCheckBox::toggled, [=](bool checked) {
        minSpinBox->setEnabled(checked);
        maxSpinBox->setEnabled(checked);
        pointsSpinBox->setEnabled(checked);
        slider->setEnabled(checked);
    });

    enableCheckBox->setChecked(false);
    minSpinBox->setEnabled(false);
    maxSpinBox->setEnabled(false);
    pointsSpinBox->setEnabled(false);
    slider->setEnabled(false);
}

void GraphWindow::createControllerParameterForms(QVBoxLayout *layout) {
    auto Kp = new QHBoxLayout;
    auto Tu = new QHBoxLayout;
    auto Td = new QHBoxLayout;

    createParameterForm("K<sub>p</sub>", Kp, 0.1, 50, 1, 10);
    createParameterForm("T<sub>u</sub>", Tu, 0.1, 2000, 1, 120);
    createParameterForm("T<sub>d</sub>", Td, 0.1, 2000, 1, 60);

    layout->addLayout(Kp);
    layout->addLayout(Tu);
    layout->addLayout(Td);
}