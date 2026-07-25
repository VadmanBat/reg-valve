#pragma once

#include <QWidget>

#include <utility>
#include <vector>

class QGridLayout;
class QLabel;
class QLineEdit;

class RegulationWidget : public QWidget {
    Q_OBJECT
public:
    explicit RegulationWidget(int rows = 3, int cols = 2, QWidget* parent = nullptr);

    void setLabels(const std::vector<QString>& labelNames);
    void setPrecisions(const std::vector<int>& valuePrecisions);
    void setColors(const std::vector<std::pair<int, int>>& valueColors);
    void updateValues(const std::vector<double>& values);

private:
    void applyDefaultStyle();
    [[nodiscard]] int getColorIndex(std::size_t index, double oldValue, double newValue) const;
    void updateCellStyle(std::size_t index, double newValue);
    static QString formatDouble(double value, int precision);

    QGridLayout* layout_{nullptr};
    std::vector<QLabel*> labels_;
    std::vector<QLineEdit*> lineEdits_;
    std::vector<double> lastValues_;
    std::vector<int> precisions_;
    std::vector<std::pair<int, int>> colors_;
    const QString palette_[3] = {"white", "#90ee90", "#ffb6c1"};
};
