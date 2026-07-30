#pragma once

#include "numina/classes/control/transfer-function.h"

#include <QDialog>

namespace Ui {
class TranFuncDialog;
}

class TranFuncDialog : public QDialog {
    Q_OBJECT
public:
    explicit TranFuncDialog(const numina::TransferFunction& tf, QWidget* parent = nullptr);
    ~TranFuncDialog() override;

private:
    void fillPoles();
    static QColor rootColor(double value);

    Ui::TranFuncDialog* ui;
    /// Own a copy so the dialog never depends on caller lifetime / moves.
    numina::TransferFunction tf_;
};
