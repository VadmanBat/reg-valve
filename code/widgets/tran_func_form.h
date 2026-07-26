#pragma once

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

#include <vector>

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;

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

    /// Machine + human-readable text for clipboard / report.
    [[nodiscard]] QString exportText() const;
    /// Apply from clipboard text (RegValve-TF-v1). Returns false if format unknown.
    bool importText(const QString& text);

    void setTransferFunction(const numina::TransferFunction* tf);
    void bindNameLabel(QLabel* label);

signals:
    void coefficientsChanged();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void copyToClipboard();
    void pasteFromClipboard();

private:
    class LineEdit;

    using VecLine = std::vector<LineEdit*>;
    using Vec     = std::vector<double>;

    QLayout* createCoeffRow(VecLine& lineEdits, class QDoubleValidator* validator, int factor);
    void createLabelContextMenu(QLabel* label);
    void updateNameLabel();
    void rebuildActivityMask();
    void setCoeffFields(VecLine& fields, const Vec& highToLow);
    void setFieldValue(LineEdit* lineEdit, double value);
    void repositionClipButtons();

    static double getValue(QString text);
    static void adjustLineEditWidth(QLineEdit* lineEdit);
    static Vec reverseOptimize(const Vec& container);
    static Vec getLineEditData(const VecLine& lineEdits);
    static QString correctLine(const QString& text);
    static QString formatCoeff(double value);
    static QString polyToString(const Vec& highToLow);

    VecLine numerator_;
    VecLine denominator_;
    QDoubleSpinBox* delayElement_{nullptr};
    QWidget* delayGroup_{nullptr};
    QPushButton* copyBtn_{nullptr};
    QPushButton* pasteBtn_{nullptr};
    std::vector<bool> is_active_;
    QLabel* nameLabel_{nullptr};
    int id_{0};
    const numina::TransferFunction* tf_{nullptr};
};
