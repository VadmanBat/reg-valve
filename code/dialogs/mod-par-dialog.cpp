#include "code/dialogs/mod-par-dialog.h"
#include "ui_mod-par-dialog.h"

ModParDialog::ModParDialog(const ModelParam& values, QWidget* parent)
    : QDialog(parent), ui(new Ui::ModParDialog) {
    ui->setupUi(this);

    ui->autoSimTimeCheckBox->setChecked(values.autoSimTime);
    ui->autoTimeIntervalsCheckBox->setChecked(values.autoTimeIntervals);
    ui->autoFreqRangeCheckBox->setChecked(values.autoFreqRange);
    ui->autoFreqIntervalsCheckBox->setChecked(values.autoFreqIntervals);
    ui->simTimeSpinBox->setValue(values.simTime);
    ui->timeIntervalsSpinBox->setValue(values.timeIntervals);
    ui->freqIntervalsSpinBox->setValue(values.freqIntervals);
    ui->approxOrderSpinBox->setValue(values.approxOrder);
    ui->freqMinSpinBox->setValue(values.freqMin);
    ui->freqMaxSpinBox->setValue(values.freqMax);
    ui->freqScaleComboBox->setCurrentIndex(values.freqScale);

    connect(ui->autoSimTimeCheckBox, &QCheckBox::toggled, this, &ModParDialog::onSimTimeToggled);
    connect(ui->autoTimeIntervalsCheckBox, &QCheckBox::toggled, this, &ModParDialog::onAutoTimeIntervalsToggled);
    connect(ui->autoFreqRangeCheckBox, &QCheckBox::toggled, this, &ModParDialog::onAutoFreqRangeToggled);
    connect(ui->autoFreqIntervalsCheckBox, &QCheckBox::toggled, this, &ModParDialog::onAutoFreqIntervalsToggled);
    connect(ui->applyButton, &QPushButton::clicked, this, &ModParDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ModParDialog::reject);

    onSimTimeToggled(values.autoSimTime);
    onAutoTimeIntervalsToggled(values.autoTimeIntervals);
    onAutoFreqRangeToggled(values.autoFreqRange);
    onAutoFreqIntervalsToggled(values.autoFreqIntervals);
}

ModParDialog::~ModParDialog() {
    delete ui;
}

ModelParam ModParDialog::data() const {
    return ModelParam{
        .autoSimTime       = ui->autoSimTimeCheckBox->isChecked(),
        .autoTimeIntervals = ui->autoTimeIntervalsCheckBox->isChecked(),
        .autoFreqRange     = ui->autoFreqRangeCheckBox->isChecked(),
        .autoFreqIntervals = ui->autoFreqIntervalsCheckBox->isChecked(),
        .simTime           = ui->simTimeSpinBox->value(),
        .timeIntervals     = ui->timeIntervalsSpinBox->value(),
        .freqIntervals     = ui->freqIntervalsSpinBox->value(),
        .approxOrder       = ui->approxOrderSpinBox->value(),
        .freqMin           = ui->freqMinSpinBox->value(),
        .freqMax           = ui->freqMaxSpinBox->value(),
        .freqScale         = ui->freqScaleComboBox->currentIndex(),
    };
}

void ModParDialog::onSimTimeToggled(bool checked) {
    ui->simTimeSpinBox->setEnabled(!checked);
    ui->autoTimeIntervalsCheckBox->setEnabled(!checked);
    if (checked)
        ui->autoTimeIntervalsCheckBox->setChecked(true);
}

void ModParDialog::onAutoTimeIntervalsToggled(bool checked) {
    ui->timeIntervalsSpinBox->setEnabled(!checked);
}

void ModParDialog::onAutoFreqRangeToggled(bool checked) {
    ui->freqMinSpinBox->setEnabled(!checked);
    ui->freqMaxSpinBox->setEnabled(!checked);
}

void ModParDialog::onAutoFreqIntervalsToggled(bool checked) {
    if (checked)
        ui->freqScaleComboBox->setCurrentIndex(0);
    ui->freqScaleComboBox->setEnabled(!checked);
    ui->freqIntervalsSpinBox->setEnabled(!checked);
}
