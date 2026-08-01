#pragma once

#include "code/model/model-param.hpp"

#include <QDialog>

namespace Ui {
class ModParDialog;
}

class ModParDialog : public QDialog {
    Q_OBJECT

private:
    Ui::ModParDialog* ui;

    void on_auto_time_range_toggled(bool checked);
    void on_auto_time_intervals_toggled(bool checked);
    void on_auto_freq_range_toggled(bool checked);
    void on_auto_freq_intervals_toggled(bool checked);

public:
    explicit ModParDialog(const ModelParam& values, QWidget* parent = nullptr);
    ~ModParDialog() override;

    [[nodiscard]] ModelParam data() const;
};
