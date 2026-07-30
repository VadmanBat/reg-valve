#pragma once

#include "code/model/id-settings.hpp"

#include <QDialog>

namespace Ui {
class IdSettingsDialog;
}

class IdSettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit IdSettingsDialog(const IdSettings& values, QWidget* parent = nullptr);
    ~IdSettingsDialog() override;

    [[nodiscard]] IdSettings data() const;

private slots:
    void onAutoOrderToggled(bool on);

private:
    Ui::IdSettingsDialog* ui;
};
