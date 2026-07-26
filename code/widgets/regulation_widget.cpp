#include "code/widgets/regulation_widget.h"

#include "code/util/style_core.hpp"

#include <QFontMetrics>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include <iomanip>
#include <sstream>

RegulationWidget::RegulationWidget(int rows, int cols, QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("RegulationWidget"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    layout_ = new QGridLayout(this);
    layout_->setContentsMargins(4, 4, 4, 4);
    layout_->setHorizontalSpacing(6);
    layout_->setVerticalSpacing(6);
    setLayout(layout_);

    const auto size = rows * cols;
    labels_.reserve(size);
    lineEdits_.reserve(size);
    lastValues_.assign(size, -1);
    precisions_.assign(size, 2);
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
            edit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            style_util::setProperty(edit, "metricFlash", 0);

            labels_.push_back(label);
            lineEdits_.push_back(edit);
            layout_->addWidget(label, i, j * 2);
            layout_->addWidget(edit, i, j * 2 + 1);
        }
        layout_->setRowStretch(i, 0);
    }

    // Label columns hug content; value columns take remaining width.
    for (int j = 0; j < cols; ++j) {
        layout_->setColumnStretch(j * 2, 0);
        layout_->setColumnStretch(j * 2 + 1, 1);
        layout_->setColumnMinimumWidth(j * 2 + 1, 64);
    }

    applyDefaultStyle();
}

void RegulationWidget::applyDefaultStyle() {
    // Heights from font so HTML subscripts (t<sub>р</sub>) are not clipped.
    const QFontMetrics fm(font());
    const int rowH = qMax(24, fm.height() + fm.descent() + 8);

    for (auto* label : labels_) {
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setTextFormat(Qt::RichText);
        label->setMinimumHeight(rowH);
        label->setMinimumWidth(0);
        // No fixed width/height — size follows rich-text sizeHint + QSS.
    }
    for (auto* edit : lineEdits_) {
        edit->setReadOnly(true);
        edit->setMinimumHeight(rowH);
        edit->setMinimumWidth(56);
    }
}

QString RegulationWidget::formatDouble(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return QString::fromStdString(out.str());
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
        // Ensure sizeHint updates for new rich text (subscripts).
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
