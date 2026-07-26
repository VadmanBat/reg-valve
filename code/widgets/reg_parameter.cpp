#include "code/widgets/reg_parameter.h"

#include <QAbstractSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

namespace {
constexpr int kLabelW   = 40;
constexpr int kCheckW   = 22;
constexpr int kSpinW    = 100;
constexpr int kSpinH    = 28;
} // namespace

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
    label_->setObjectName(QStringLiteral("regParamLabel"));
    checkBox_->setObjectName(QStringLiteral("regParamCheck"));
    minSpinBox_->setObjectName(QStringLiteral("regParamMin"));
    maxSpinBox_->setObjectName(QStringLiteral("regParamMax"));
    pointsSpinBox_->setObjectName(QStringLiteral("regParamPoints"));
    slider_->setObjectName(QStringLiteral("regParamSlider"));

    // Fixed columns so Kp / Tu / Td rows stack and align.
    label_->setFixedWidth(kLabelW);
    label_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    checkBox_->setFixedWidth(kCheckW);
    checkBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    minSpinBox_->setRange(min, max);
    minSpinBox_->setDecimals(2);
    minSpinBox_->setValue(minValue);
    minSpinBox_->setPrefix(tr("от: "));
    minSpinBox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    minSpinBox_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    maxSpinBox_->setRange(min, max);
    maxSpinBox_->setDecimals(2);
    maxSpinBox_->setValue(maxValue);
    maxSpinBox_->setPrefix(tr("до: "));
    maxSpinBox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    maxSpinBox_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    pointsSpinBox_->setRange(10, 2000);
    pointsSpinBox_->setPrefix(tr("точки: "));
    pointsSpinBox_->setValue(1000);
    pointsSpinBox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    pointsSpinBox_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

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

    auto* row = qobject_cast<QHBoxLayout*>(layout_);
    row->setContentsMargins(0, 4, 0, 4);
    row->setSpacing(8);
    row->setAlignment(Qt::AlignVCenter);

    row->addWidget(label_, 0, Qt::AlignVCenter);
    row->addWidget(checkBox_, 0, Qt::AlignVCenter);
    row->addWidget(minSpinBox_, 0, Qt::AlignVCenter);
    row->addWidget(maxSpinBox_, 0, Qt::AlignVCenter);
    row->addWidget(pointsSpinBox_, 0, Qt::AlignVCenter);
    row->addWidget(slider_, 1);

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
    minSpinBox_->setFixedSize(kSpinW, kSpinH);
    maxSpinBox_->setFixedSize(kSpinW, kSpinH);
    pointsSpinBox_->setFixedSize(kSpinW, kSpinH);
}

bool RegParameter::enabled() const {
    return checkBox_->isChecked();
}

double RegParameter::value() const {
    return slider_->value();
}
