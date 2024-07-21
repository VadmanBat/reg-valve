//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

QString Application::getColor(const double& value) {
    if (value == 0)
        return "violet";
    return value > 0 ? "blue" : "red";
}

double Application::getValue(QString text) {
    text.replace(',', '.');
    if (text.count() == 1)
        text += '1';
    return text.toDouble();
}

void Application::adjustLineEditWidth(QLineEdit* lineEdit) {
    int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 10);
    lineEdit->setMinimumWidth(width);
    lineEdit->setMaximumWidth(width);
}

MathCore::Vec Application::getLineEditData(const std::vector <LineEdit*>& lineEdits) {
    const auto count(lineEdits.size());
    MathCore::Vec values(count);
    for (int i = 0; i < count; ++i)
        values[i] = getValue(lineEdits[i]->text());
    return reverseOptimize(values);
}

QLayout* Application::createLineEdit(const QString& name, std::vector <LineEdit*>& lineEdits, QDoubleValidator* validator) {
    auto layout = new QHBoxLayout;
    layout->addWidget(new QLabel(name));

    int p(-1);
    for (auto& lineEdit : lineEdits) {
        layout->addWidget(lineEdit = new LineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText("+0");

        auto label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
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

    return layout;
}

QLayout* Application::createTransferFunctionForm(std::vector <LineEdit*>& numerator, std::vector <LineEdit*>& denominator,
                                                 std::size_t n, std::size_t m, const QString& title) {
    auto transferFunctionLabel = new QLabel(title);
    transferFunctionLabel->setAlignment(Qt::AlignCenter);
    transferFunctionLabel->setStyleSheet("font-size: 24pt;");

    numerator.resize(n);
    denominator.resize(m);
    auto realNumberValidator = new QDoubleValidator;
    realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMidLineWidth(10);

    auto transferFunctionLayout = new QVBoxLayout;
    transferFunctionLayout->addLayout(createLineEdit("Числитель:\t", numerator, realNumberValidator));
    transferFunctionLayout->addWidget(line);
    transferFunctionLayout->addLayout(createLineEdit("Знаменатель:\t", denominator, realNumberValidator));

    auto layout = new QHBoxLayout;
    layout->addWidget(transferFunctionLabel);
    layout->addLayout(transferFunctionLayout);

    return layout;
}

void Application::updateStyleSheetProperty(QLineEdit* lineEdit, const QString& property, const QString& value) {
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

QString Application::correctLine(const QString& text) {
    if (text.at(0) == ',')
        return "+0" + text;
    if (text.at(0).isDigit())
        return '+' + text;
    if (text.count() > 1 && text.at(1) == ',')
        return text.at(0) + '0' + text.mid(1);
    return "";
}

void Application::updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, DoubleSlider *slider) {
    double min = minSpinBox->value();
    double max = maxSpinBox->value();
    int points = pointsSpinBox->value();

    if (min < max && points > 0) {
        slider->setRange(min, max);
        slider->setSingleStep((max - min) / (points - 1));
    }
}

void Application::setSpinBox(QDoubleSpinBox* spinBox, double min, double max, double value, const char* prefix) {
    spinBox->setRange(min, max);
    spinBox->setDecimals(2);
    spinBox->setValue(value);
    spinBox->setPrefix(prefix);
}

std::pair <DoubleSlider*, QLayout*> Application::createSliderForm(const SliderData& data) {
    const auto [title, lower, upper, min, max] = data;

    auto layout = new QHBoxLayout;
    auto titleLabel = new QLabel(title);
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

    return {slider, layout};
}

QLayout* Application::createSlidersForm(std::vector <DoubleSlider*>& sliders, const SlidersDataset& dataset) {
    sliders.reserve(dataset.size());
    auto layout = new QVBoxLayout;
    for (const auto& sliderData : dataset) {
        auto [slider, sliderLayout](createSliderForm(sliderData));
        sliders.emplace_back(slider);
        layout->addLayout(sliderLayout);
    }
    return layout;
}