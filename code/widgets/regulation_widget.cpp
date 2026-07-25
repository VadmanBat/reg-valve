#include "code/widgets/regulation_widget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>

#include <iomanip>
#include <sstream>

RegulationWidget::RegulationWidget(int rows, int cols, QWidget* parent) : QWidget(parent) {
    layout_ = new QGridLayout(this);
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
            auto* edit  = new QLineEdit(this);
            labels_.push_back(label);
            lineEdits_.push_back(edit);
            layout_->addWidget(label, i, j * 2);
            layout_->addWidget(edit, i, j * 2 + 1);
        }
    }
    applyDefaultStyle();
}

void RegulationWidget::applyDefaultStyle() {
    for (auto* label : labels_) {
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setStyleSheet("font-size: 20px;");
        label->setFixedSize(48, 24);
    }
    for (auto* edit : lineEdits_) {
        edit->setStyleSheet("font-size: 20px;");
        edit->setMinimumSize(80, 24);
        edit->setReadOnly(true);
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
    QString currentStyle = lineEdits_[index]->styleSheet();
    const int colorIndex = getColorIndex(index, lastValues_[index], newValue);
    const QString newStyle = QString("background-color: %1;").arg(palette_[colorIndex]);
    static const QRegularExpression regex("background-color: [^;]+;");
    if (regex.match(currentStyle).hasMatch())
        currentStyle.replace(regex, newStyle);
    else
        currentStyle += newStyle;
    lineEdits_[index]->setStyleSheet(currentStyle);
}

void RegulationWidget::setLabels(const std::vector<QString>& labelNames) {
    const auto n = std::min(labels_.size(), labelNames.size());
    for (std::size_t i = 0; i < n; ++i)
        labels_[i]->setText(labelNames[i]);
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
