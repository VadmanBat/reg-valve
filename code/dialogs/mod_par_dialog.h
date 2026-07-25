#pragma once

#include "code/model/model_param.hpp"

#include <QDialog>

namespace Ui {
class ModParDialog;
}

class ModParDialog : public QDialog {
    Q_OBJECT
public:
    explicit ModParDialog(const ModelParam& values, QWidget* parent = nullptr);
    ~ModParDialog() override;

    [[nodiscard]] ModelParam data() const;

private slots:
    void onSimTimeToggled(bool checked);
    void onAutoTimeIntervalsToggled(bool checked);
    void onAutoFreqRangeToggled(bool checked);
    void onAutoFreqIntervalsToggled(bool checked);

private:
    Ui::ModParDialog* ui;
};
