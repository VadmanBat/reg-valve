#include "code/dialogs/id-settings-dialog.h"
#include "ui_id-settings-dialog.h"

IdSettingsDialog::IdSettingsDialog(const IdSettings& values, QWidget* parent)
    : QDialog(parent), ui(new Ui::IdSettingsDialog) {
    ui->setupUi(this);

    ui->autoOrderCheck->setChecked(values.autoOrder);
    ui->denOrderSpin->setValue(values.denOrder);
    ui->numOrderSpin->setValue(values.numOrder);
    ui->estimateTauCheck->setChecked(values.estimateTau);

    connect(ui->autoOrderCheck, &QCheckBox::toggled, this, &IdSettingsDialog::onAutoOrderToggled);
    connect(ui->applyButton, &QPushButton::clicked, this, &IdSettingsDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &IdSettingsDialog::reject);

    onAutoOrderToggled(values.autoOrder);
}

IdSettingsDialog::~IdSettingsDialog() {
    delete ui;
}

IdSettings IdSettingsDialog::data() const {
    IdSettings s;
    s.autoOrder   = ui->autoOrderCheck->isChecked();
    s.denOrder    = ui->denOrderSpin->value();
    s.numOrder    = ui->numOrderSpin->value();
    s.estimateTau = ui->estimateTauCheck->isChecked();
    if (s.numOrder > s.denOrder)
        s.numOrder = s.denOrder;
    return s;
}

void IdSettingsDialog::onAutoOrderToggled(bool on) {
    ui->denOrderSpin->setEnabled(!on);
    ui->numOrderSpin->setEnabled(!on);
    ui->denOrderLabel->setEnabled(!on);
    ui->numOrderLabel->setEnabled(!on);
}
