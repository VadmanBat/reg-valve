#include "code/widgets/regulation-widget.h"

#include "code/util/format.hxx"
#include "code/util/style-core.hpp"

#include <QFontMetrics>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

RegulationWidget::RegulationWidget(int rows, int cols, QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("RegulationWidget"));
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    layout_ = new QGridLayout(this);
    layout_->setContentsMargins(4, 4, 4, 4);
    layout_->setHorizontalSpacing(6);
    layout_->setVerticalSpacing(6);
    setLayout(layout_);

    const auto size = rows * cols;
    labels_.reserve(size);
    lineEdits_.reserve(size);
    lastValues_.assign(size, -1);
    precisions_.assign(size, 0);
    colors_.assign(size, {1, 2});

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            auto* label = new QLabel(this);
            label->setObjectName(QStringLiteral("metricLabel"));
            label->setTextFormat(Qt::RichText);
            label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
            label->setWordWrap(false);

            auto* edit = new QLineEdit(this);
            edit->setObjectName(QStringLiteral("metricValue"));
            edit->setReadOnly(true);
            edit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
            style_util::setProperty(edit, "metricFlash", 0);

            labels_.push_back(label);
            lineEdits_.push_back(edit);
            layout_->addWidget(label, i, j * 2);
            layout_->addWidget(edit, i, j * 2 + 1);
        }
        layout_->setRowStretch(i, 0);
    }

    for (int j = 0; j < cols; ++j) {
        layout_->setColumnStretch(j * 2, 0);
        layout_->setColumnStretch(j * 2 + 1, 0);
    }

    applyDefaultStyle();
}

void RegulationWidget::applyDefaultStyle() {
    const QFontMetrics fm(font());
    const int rowH = qMax(24, fm.height() + fm.descent() + 8);
    // Fixed width + 1 extra glyph for scientific like -1.23456e+123
    const int valW = fm.horizontalAdvance(QStringLiteral("-1.23456e+1230")) + 16;

    for (auto* label : labels_) {
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setTextFormat(Qt::RichText);
        label->setMinimumHeight(rowH);
        label->setMinimumWidth(0);
    }
    for (auto* edit : lineEdits_) {
        edit->setReadOnly(true);
        edit->setMinimumHeight(rowH);
        edit->setFixedWidth(valW);
    }
}

QString RegulationWidget::formatDouble(double value, int /*precision*/) {
    return num_format::format(value, num_format::SIGNIFICANT_DIGITS);
}

int RegulationWidget::getColorIndex(std::size_t index, double oldValue, double newValue) const {
    if (oldValue == -1)
        return 0;
    if (newValue < oldValue)
        return colors_[index].first;
    if (newValue > oldValue)
        return colors_[index].second;
    return 0;
}

void RegulationWidget::updateCellStyle(std::size_t index, double newValue) {
    const int flash = getColorIndex(index, lastValues_[index], newValue);
    style_util::setProperty(lineEdits_[index], "metricFlash", flash);
}

void RegulationWidget::setLabels(const std::vector<QString>& labelNames) {
    const auto n = std::min(labels_.size(), labelNames.size());
    for (std::size_t i = 0; i < n; ++i) {
        labels_[i]->setText(labelNames[i]);
        labels_[i]->adjustSize();
        labels_[i]->updateGeometry();
    }
    updateGeometry();
}

void RegulationWidget::setPrecisions(const std::vector<int>& valuePrecisions) {
    const auto n = std::min(precisions_.size(), valuePrecisions.size());
    for (std::size_t i = 0; i < n; ++i)
        precisions_[i] = valuePrecisions[i];
}

void RegulationWidget::setColors(const std::vector<std::pair<int, int>>& valueColors) {
    const auto n = std::min(colors_.size(), valueColors.size());
    for (std::size_t i = 0; i < n; ++i)
        colors_[i] = valueColors[i];
}

void RegulationWidget::updateValues(const std::vector<double>& values) {
    if (values.empty()) {
        for (std::size_t i = 0; i < lineEdits_.size(); ++i) {
            lineEdits_[i]->setText({});
            lastValues_[i] = -1;
            style_util::setProperty(lineEdits_[i], "metricFlash", 0);
        }
        return;
    }
    const auto n = std::min(lineEdits_.size(), values.size());
    for (std::size_t i = 0; i < n; ++i) {
        updateCellStyle(i, values[i]);
        lineEdits_[i]->setText(formatDouble(values[i], precisions_[i]));
        lastValues_[i] = values[i];
    }
}
