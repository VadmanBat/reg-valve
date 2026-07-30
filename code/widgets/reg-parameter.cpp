#include "code/widgets/reg-parameter.h"

#include <QAbstractSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

namespace {
constexpr int kLabelW = 40;
constexpr int kCheckW = 22;
constexpr int kSpinW  = 100;
constexpr int kSpinH  = 28;
} // namespace

RegParameter::RegParameter(const QString& title, double min, double max, double minValue, double maxValue,
                           QObject* parent)
    : QObject(parent),
      label_(new QLabel(title)),
      check_box_(new QCheckBox),
      min_spin_(new QDoubleSpinBox),
      max_spin_(new QDoubleSpinBox),
      points_spin_(new QSpinBox),
      slider_(new DoubleSlider(Qt::Horizontal)),
      layout_(new QHBoxLayout) {
    label_->setObjectName(QStringLiteral("regParamLabel"));
    check_box_->setObjectName(QStringLiteral("regParamCheck"));
    min_spin_->setObjectName(QStringLiteral("regParamMin"));
    max_spin_->setObjectName(QStringLiteral("regParamMax"));
    points_spin_->setObjectName(QStringLiteral("regParamPoints"));
    slider_->setObjectName(QStringLiteral("regParamSlider"));

    label_->setFixedWidth(kLabelW);
    label_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    check_box_->setFixedWidth(kCheckW);
    check_box_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    min_spin_->setRange(min, max);
    min_spin_->setDecimals(2);
    min_spin_->setValue(minValue);
    min_spin_->setPrefix(tr("от: "));
    min_spin_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    min_spin_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    max_spin_->setRange(min, max);
    max_spin_->setDecimals(2);
    max_spin_->setValue(maxValue);
    max_spin_->setPrefix(tr("до: "));
    max_spin_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    max_spin_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    points_spin_->setRange(10, 2000);
    points_spin_->setPrefix(tr("точки: "));
    points_spin_->setValue(1000);
    points_spin_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    points_spin_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    update_slider_range();
    enable(false);

    connect(min_spin_, &QDoubleSpinBox::editingFinished, this, [this] {
        if (min_spin_->value() > max_spin_->value())
            max_spin_->setValue(min_spin_->value());
        else
            update_slider_range();
    });
    connect(max_spin_, &QDoubleSpinBox::editingFinished, this, [this] {
        if (min_spin_->value() > max_spin_->value())
            min_spin_->setValue(max_spin_->value());
        else
            update_slider_range();
    });
    connect(points_spin_, &QSpinBox::editingFinished, this, &RegParameter::update_slider_range);
    connect(check_box_, &QCheckBox::toggled, this, &RegParameter::enable);

    auto* row = qobject_cast<QHBoxLayout*>(layout_);
    row->setContentsMargins(0, 4, 0, 4);
    row->setSpacing(8);
    row->setAlignment(Qt::AlignVCenter);

    row->addWidget(label_, 0, Qt::AlignVCenter);
    row->addWidget(check_box_, 0, Qt::AlignVCenter);
    row->addWidget(min_spin_, 0, Qt::AlignVCenter);
    row->addWidget(max_spin_, 0, Qt::AlignVCenter);
    row->addWidget(points_spin_, 0, Qt::AlignVCenter);
    row->addWidget(slider_, 1);

    apply_default_style();
}

void RegParameter::update_slider_range() {
    const auto min    = min_spin_->value();
    const auto max    = max_spin_->value();
    const auto points = points_spin_->value();
    if (min < max) {
        const auto value = slider_->value();
        slider_->setRange(min, max, points - 1);
        slider_->setValue(value);
    }
}

void RegParameter::enable(bool checked) {
    min_spin_->setEnabled(checked);
    max_spin_->setEnabled(checked);
    points_spin_->setEnabled(checked);
    slider_->setEnabled(checked);
}

void RegParameter::apply_default_style() {
    min_spin_->setFixedSize(kSpinW, kSpinH);
    max_spin_->setFixedSize(kSpinW, kSpinH);
    points_spin_->setFixedSize(kSpinW, kSpinH);
}

bool RegParameter::enabled() const {
    return check_box_->isChecked();
}

double RegParameter::value() const {
    return slider_->value();
}
