#include "code/dialogs/id-settings-dialog.h"

#include "code/util/dialog-icons.hxx"
#include "code/util/secondary-text.hxx"
#include "ui_id-settings-dialog.h"

IdSettingsDialog::IdSettingsDialog(const IdSettings& values, QWidget* parent)
    : QDialog(parent), ui(new Ui::IdSettingsDialog) {
    ui->setupUi(this);
    dialog_icons::apply(this, dialog_icons::Kind::IdSettings);
    secondary_text::applyAll({ui->hintLabel, ui->structHint, ui->delayHint});

    ui->autoOrderCheck->setChecked(values.autoOrder);
    ui->denOrderSpin->setValue(values.denOrder);
    ui->numOrderSpin->setValue(values.numOrder);
    ui->estimateTauCheck->setChecked(values.estimateTau);

    connect(ui->autoOrderCheck, &QCheckBox::toggled, this, &IdSettingsDialog::onAutoOrderToggled);
    connect(ui->estimateTauCheck, &QCheckBox::toggled, this, [this](bool on) {
        ui->delayHint->setText(on ? tr("Запаздывание τ будет оценено по данным.")
                                  : tr("Модель без запаздывания (τ = 0)."));
    });
    connect(ui->applyButton, &QPushButton::clicked, this, &IdSettingsDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &IdSettingsDialog::reject);

    onAutoOrderToggled(values.autoOrder);
    ui->delayHint->setText(values.estimateTau ? tr("Запаздывание τ будет оценено по данным.")
                                              : tr("Модель без запаздывания (τ = 0)."));
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
    // Progressive disclosure: manual n/m only when structure is user-defined.
    ui->denOrderSpin->setEnabled(!on);
    ui->numOrderSpin->setEnabled(!on);
    ui->denOrderLabel->setEnabled(!on);
    ui->numOrderLabel->setEnabled(!on);
    ui->structHint->setText(on ? tr("Структура подбирается автоматически.")
                               : tr("Задайте порядки знаменателя n и числителя m вручную (m ≤ n)."));
}
