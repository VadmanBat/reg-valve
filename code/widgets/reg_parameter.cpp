#include "code/widgets/reg_parameter.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

RegParameter::RegParameter(const QString& title, double min, double max, double minValue, double maxValue,
                           QObject* parent)
    : QObject(parent),
      label_(new QLabel(title)),
      checkBox_(new QCheckBox),
      minSpinBox_(new QDoubleSpinBox),
      maxSpinBox_(new QDoubleSpinBox),
      pointsSpinBox_(new QSpinBox),
      slider_(new DoubleSlider(Qt::Horizontal)),
      layout_(new QHBoxLayout) {
    minSpinBox_->setRange(min, max);
    minSpinBox_->setDecimals(2);
    minSpinBox_->setValue(minValue);
    minSpinBox_->setPrefix(tr("от: "));

    maxSpinBox_->setRange(min, max);
    maxSpinBox_->setDecimals(2);
    maxSpinBox_->setValue(maxValue);
    maxSpinBox_->setPrefix(tr("до: "));

    pointsSpinBox_->setRange(10, 2000);
    pointsSpinBox_->setPrefix(tr("точки: "));
    pointsSpinBox_->setValue(1000);

    updateSliderRange();
    enable(false);

    connect(minSpinBox_, &QDoubleSpinBox::editingFinished, this, [this] {
        if (minSpinBox_->value() > maxSpinBox_->value())
            maxSpinBox_->setValue(minSpinBox_->value());
        else
            updateSliderRange();
    });
    connect(maxSpinBox_, &QDoubleSpinBox::editingFinished, this, [this] {
        if (minSpinBox_->value() > maxSpinBox_->value())
            minSpinBox_->setValue(maxSpinBox_->value());
        else
            updateSliderRange();
    });
    connect(pointsSpinBox_, &QSpinBox::editingFinished, this, &RegParameter::updateSliderRange);
    connect(checkBox_, &QCheckBox::toggled, this, &RegParameter::enable);

    layout_->addWidget(label_);
    layout_->addWidget(checkBox_);
    layout_->addWidget(minSpinBox_);
    layout_->addWidget(maxSpinBox_);
    layout_->addWidget(pointsSpinBox_);
    layout_->addWidget(slider_);
    applyDefaultStyle();
}

void RegParameter::updateSliderRange() {
    const auto min = minSpinBox_->value();
    const auto max = maxSpinBox_->value();
    const auto points = pointsSpinBox_->value();
    if (min < max) {
        const auto value = slider_->value();
        slider_->setRange(min, max, points - 1);
        slider_->setValue(value);
    }
}

void RegParameter::enable(bool checked) {
    minSpinBox_->setEnabled(checked);
    maxSpinBox_->setEnabled(checked);
    pointsSpinBox_->setEnabled(checked);
    slider_->setEnabled(checked);
}

void RegParameter::applyDefaultStyle() {
    label_->setStyleSheet("font-size: 16pt;");
    minSpinBox_->setStyleSheet("font-size: 9pt;");
    maxSpinBox_->setStyleSheet("font-size: 9pt;");
    pointsSpinBox_->setStyleSheet("font-size: 9pt;");
    minSpinBox_->setFixedSize(100, 30);
    maxSpinBox_->setFixedSize(100, 30);
    pointsSpinBox_->setFixedSize(100, 30);
}

bool RegParameter::enabled() const {
    return checkBox_->isChecked();
}

double RegParameter::value() const {
    return slider_->value();
}
