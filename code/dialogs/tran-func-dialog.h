#pragma once

#include "numina/classes/control/transfer-function.h"

#include <QDialog>

namespace Ui {
class TranFuncDialog;
}

class TranFuncDialog : public QDialog {
    Q_OBJECT

private:
    Ui::TranFuncDialog* ui;
    /// Own a copy so the dialog never depends on caller lifetime / moves.
    numina::TransferFunction tf_;

    void fill_poles();
    void setup_copy_menus();
    void copy_solution_text(const QString& text, QWidget* anchor);
    static QColor root_color(double value);

public:
    explicit TranFuncDialog(const numina::TransferFunction& tf, QWidget* parent = nullptr);
    ~TranFuncDialog() override;
};
