#pragma once

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

#include <vector>

class QLabel;
class QPushButton;

/// Read-only W(p)=N/D·e^{-τp}: HTML poly text, copy only (button under delay, like TranFuncForm).
class TfDisplayWidget : public QWidget {
    Q_OBJECT
public:
    using Vec = std::vector<double>;

private:
    QLabel* titleLabel_{nullptr};
    QLabel* numLabel_{nullptr};
    QLabel* denLabel_{nullptr};
    QLabel* delayLabel_{nullptr};
    QWidget* delayGroup_{nullptr};
    QPushButton* copyBtn_{nullptr};
    bool empty_{true};
    Vec num_;
    Vec den_;
    double tau_{0.0};

    void set_polys(const Vec& num, const Vec& den, double tau);
    void reposition_copy_button();

private slots:
    void copyToClipboard();

public:
    explicit TfDisplayWidget(const QString& title = QStringLiteral("W(p) = "), QWidget* parent = nullptr);

    void setTransferFunction(const numina::TransferFunction& tf, double tau = 0.0);
    void clear();

    [[nodiscard]] bool isEmpty() const noexcept { return empty_; }
    [[nodiscard]] double delayTime() const noexcept { return tau_; }
    [[nodiscard]] QString exportText() const;
    [[nodiscard]] QString humanText() const;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
};
