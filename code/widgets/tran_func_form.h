#pragma once

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

#include <vector>

class QDoubleSpinBox;
class QLabel;
class QLineEdit;

/// Editable W(p) = N/D · e^{-τp}. Dynamic degree fields (not pure .ui).
class TranFuncForm : public QWidget {
    Q_OBJECT
public:
    explicit TranFuncForm(std::size_t n = 6, std::size_t m = 6, const QString& title = QStringLiteral("W(p) = "),
                          QWidget* parent = nullptr);

    [[nodiscard]] std::vector<double> numerator() const;
    [[nodiscard]] std::vector<double> denominator() const;
    [[nodiscard]] bool hasDelay() const;
    [[nodiscard]] double delayTime() const;
    [[nodiscard]] QString linkName() const;

    void setTransferFunction(const numina::TransferFunction* tf);
    void bindNameLabel(QLabel* label);

signals:
    void coefficientsChanged();

private:
    class LineEdit;

    using VecLine = std::vector<LineEdit*>;
    using Vec     = std::vector<double>;

    QLayout* createCoeffRow(VecLine& lineEdits, class QDoubleValidator* validator, int factor);
    void createLabelContextMenu(QLabel* label);
    void updateNameLabel();

    static QString getColor(double value);
    static double getValue(QString text);
    static void adjustLineEditWidth(QLineEdit* lineEdit);
    static Vec reverseOptimize(const Vec& container);
    static Vec getLineEditData(const VecLine& lineEdits);
    static QString correctLine(const QString& text);

    VecLine numerator_;
    VecLine denominator_;
    QDoubleSpinBox* delayElement_{nullptr};
    std::vector<bool> is_active_;
    QLabel* nameLabel_{nullptr};
    int id_{0};
    const numina::TransferFunction* tf_{nullptr};
};
