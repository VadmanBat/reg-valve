//
// Created by Vadma on 27.07.2024.
//

#ifndef REGVALVE_REG_WIDGET_HPP
#define REGVALVE_REG_WIDGET_HPP

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "../../style-core.hpp"

class RegulationWidget : public QWidget {
private:
    QGridLayout* layout;
    std::vector <QLabel*> labels;
    std::vector <QLineEdit*> lineEdits;
    std::vector <double> lastValues;
    std::vector <int> precisions;
    std::vector <std::pair <int, int>> colors;

    const QString palette[3] = {"white", "#90ee90", "#ffb6c1"};

    static inline QString formatDouble(double value, int precision) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(precision) << value;
        return QString::fromStdString(out.str());
    }

    void applyDefaultStyle() {
        for (auto label : labels) {
            label->setAlignment(Qt::AlignRight);
            label->setStyleSheet("font-size: 14pt;");
            label->setFixedSize(48, 24);
        }
        for (auto lineEdit : lineEdits) {
            lineEdit->setStyleSheet("font-size: 14pt;");
            lineEdit->setMinimumSize(80, 24);
            lineEdit->setReadOnly(true);
        }
    }

    inline int getColorIndex(int index, double oldValue, double newValue) {
        if (oldValue == -1)
            return 0;
        if (newValue < oldValue)
            return colors[index].first;
        if (newValue > oldValue)
            return colors[index].second;
        return 0;
    }

    void updateCellStyle(int index, double newValue) {
        QString currentStyle = lineEdits[index]->styleSheet();
        const int colorIndex = getColorIndex(index, lastValues[index], newValue);
        QString newStyle = QString("background-color: %1;").arg(palette[colorIndex]);

        static QRegularExpression regex("background-color: [^;]+;");
        if (regex.match(currentStyle).hasMatch())
            currentStyle.replace(regex, newStyle);
        else
            currentStyle += newStyle;

        lineEdits[index]->setStyleSheet(currentStyle);
    }

public:
    explicit RegulationWidget(int rows, int cols, QWidget* parent = nullptr) : QWidget(parent) {
        setLayout(layout = new QGridLayout(this));
        const auto size(rows * cols);
        labels.reserve(size);
        lineEdits.reserve(size);
        lastValues.assign(size, -1);
        precisions.assign(size, 2);
        colors.assign(size, {1, 2});
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                auto label = new QLabel(this);
                auto lineEdit = new QLineEdit(this);
                labels.push_back(label);
                lineEdits.push_back(lineEdit);
                layout->addWidget(label, i, j * 2);
                layout->addWidget(lineEdit, i, j * 2 + 1);
            }
        applyDefaultStyle();
    }

    void setLabels(const std::vector <QString>& labelNames) {
        const auto n(std::min(labels.size(), labelNames.size()));
        for (int i = 0; i < n; ++i)
            labels[i]->setText(labelNames[i]);
    }

    void setPrecisions(const std::vector <int>& valuePrecisions) {
        const auto n(std::min(labels.size(), valuePrecisions.size()));
        for (int i = 0; i < n; ++i)
            precisions[i] = valuePrecisions[i];
    }

    void setColors(const std::vector <std::pair <int, int>>& valueColors) {
        const auto n(std::min(labels.size(), valueColors.size()));
        for (int i = 0; i < n; ++i)
            colors[i] = valueColors[i];
    }

    void updateValues(const std::vector <double>& values) {
        if (values.empty()) {
            const auto n(lineEdits.size());
            for (int i = 0; i < n; ++i) {
                lineEdits[i]->setText("");
                lastValues[i] = -1;
            }
            return;
        }

        const auto n(std::min(lineEdits.size(), values.size()));
        for (int i = 0; i < n; ++i) {
            updateCellStyle(i, values[i]);
            lineEdits[i]->setText(formatDouble(values[i], precisions[i]));
            lastValues[i] = values[i];
        }
    }
};

#endif //REGVALVE_REG_WIDGET_HPP
