#pragma once

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

#include <vector>

class QLabel;
class QPushButton;

/// Read-only W(p) = N/D display: coefficients as text (no edit fields). Copy only.
class TfDisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit TfDisplayWidget(const QString& title = QStringLiteral("W(p) = "), QWidget* parent = nullptr);

    void setTransferFunction(const numina::TransferFunction& tf);
    void clear();

    [[nodiscard]] bool isEmpty() const { return empty_; }
    [[nodiscard]] QString exportText() const;
    [[nodiscard]] QString humanText() const;

private slots:
    void copyToClipboard();

private:
    using Vec = std::vector<double>;

    static QString formatCoeff(double value);
    static QString polyToString(const Vec& highToLow);
    void setPolys(const Vec& num, const Vec& den, double tau);

    QLabel* titleLabel_{nullptr};
    QLabel* numLabel_{nullptr};
    QLabel* denLabel_{nullptr};
    QLabel* delayLabel_{nullptr};
    QPushButton* copyBtn_{nullptr};
    bool empty_{true};
    Vec num_;
    Vec den_;
    double tau_{0.0};
};
