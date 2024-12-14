//
// Created by Vadma on 13.12.2024.
//

#ifndef REGVALVE_MOD_PAR_DIALOG_HPP
#define REGVALVE_MOD_PAR_DIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <cmath>

class ModParDialog : public QDialog {
    Q_OBJECT

private:
    QSpinBox *simulationTimeSpinBox;
    QCheckBox *simulationTimeCheckBox;
    QDoubleSpinBox *integrationStepDoubleSpinBox;
    QSpinBox *numberOfIntervalsSpinBox;
    QCheckBox *autoIntegrationCheckBox;
    QDoubleSpinBox *frequencyMinSpinBox;
    QDoubleSpinBox *frequencyMaxSpinBox;
    QCheckBox *autoFrequencyRangeCheckBox;
    QComboBox *frequencyScaleComboBox;
    QDoubleSpinBox *frequencyStepDoubleSpinBox;
    QSpinBox *numberOfPointsFreqSpinBox;
    QCheckBox *autoFrequencyPointsCheckBox;

public:
    explicit ModParDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Параметры моделирования");

        auto simulationTimeLabel        = new QLabel("Время моделирования (с):");
        auto integrationStepLabel       = new QLabel("Шаг интегрирования (с):");
        auto numberOfIntervalsLabel     = new QLabel("Интервалы:");
        auto frequencyScaleLabel        = new QLabel("Шкала:");
        auto frequencyRangeLabel        = new QLabel("Диапазон частот (Гц):");
        auto frequencyStepLabel         = new QLabel("Шаг (Гц):");
        auto numberOfPointsFreqLabel    = new QLabel("Количество точек:");

        simulationTimeSpinBox           = new QSpinBox;
        integrationStepDoubleSpinBox    = new QDoubleSpinBox;
        simulationTimeCheckBox          = new QCheckBox("Авто");
        numberOfIntervalsSpinBox        = new QSpinBox;
        autoIntegrationCheckBox         = new QCheckBox("Авто");
        frequencyMinSpinBox             = new QDoubleSpinBox;
        frequencyMaxSpinBox             = new QDoubleSpinBox;
        autoFrequencyRangeCheckBox      = new QCheckBox("Авто");
        frequencyScaleComboBox          = new QComboBox;
        frequencyStepDoubleSpinBox      = new QDoubleSpinBox;
        numberOfPointsFreqSpinBox       = new QSpinBox;
        autoFrequencyPointsCheckBox     = new QCheckBox("Авто");

        simulationTimeSpinBox->setMinimum(10);
        simulationTimeSpinBox->setMaximum(10000);
        simulationTimeSpinBox->setValue(500);

        connect(simulationTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModParDialog::onSimulationTimeChanged);
        connect(simulationTimeCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onSimulationTimeCheckBoxChanged);
        connect(integrationStepDoubleSpinBox, &QDoubleSpinBox::editingFinished, this, &ModParDialog::onIntegrationStepChanged);
        connect(numberOfIntervalsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModParDialog::onNumberOfPointsChanged);
        connect(autoIntegrationCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoIntegrationCheckBoxChanged);
        connect(autoFrequencyRangeCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoFrequencyRangeCheckBoxChanged);
        connect(frequencyScaleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModParDialog::onFrequencyScaleChanged);
        connect(autoFrequencyPointsCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoFrequencyPointsCheckBoxChanged);

        simulationTimeCheckBox->setCheckState(Qt::Checked);
        autoFrequencyRangeCheckBox->setCheckState(Qt::Checked);

        integrationStepDoubleSpinBox->setMinimum(0.25);
        integrationStepDoubleSpinBox->setMaximum(50);
        integrationStepDoubleSpinBox->setSingleStep(0.1);
        integrationStepDoubleSpinBox->setValue(5);

        numberOfIntervalsSpinBox->setMinimum(10);
        numberOfIntervalsSpinBox->setMaximum(2000);
        numberOfIntervalsSpinBox->setValue(100);

        frequencyScaleComboBox->addItem("Логарифмическая");
        frequencyScaleComboBox->addItem("Линейная");

        frequencyMinSpinBox->setMinimum(0.01);
        frequencyMinSpinBox->setMaximum(200);
        frequencyMinSpinBox->setValue(0.1);
        frequencyMinSpinBox->setPrefix("от: ");
        frequencyMaxSpinBox->setMinimum(0.01);
        frequencyMaxSpinBox->setMaximum(200);
        frequencyMaxSpinBox->setValue(1);
        frequencyMaxSpinBox->setPrefix("до: ");

        frequencyStepDoubleSpinBox->setEnabled(false);
        frequencyStepDoubleSpinBox->setMinimum(0.01);
        frequencyStepDoubleSpinBox->setMaximum(2);
        frequencyStepDoubleSpinBox->setValue(0.1);
        frequencyStepDoubleSpinBox->setSingleStep(0.01);

        numberOfPointsFreqSpinBox->setMinimum(2);
        numberOfPointsFreqSpinBox->setMaximum(2000);
        numberOfPointsFreqSpinBox->setValue(100);

        auto mainLayout = new QVBoxLayout;
        auto timeLayout = new QHBoxLayout;
        timeLayout->addWidget(simulationTimeLabel);
        timeLayout->addWidget(simulationTimeSpinBox);
        timeLayout->addWidget(simulationTimeCheckBox);
        mainLayout->addLayout(timeLayout);

        auto integrationLayout = new QHBoxLayout;
        integrationLayout->addWidget(integrationStepLabel);
        integrationLayout->addWidget(integrationStepDoubleSpinBox);
        integrationLayout->addWidget(numberOfIntervalsLabel);
        integrationLayout->addWidget(numberOfIntervalsSpinBox);
        integrationLayout->addWidget(autoIntegrationCheckBox);
        mainLayout->addLayout(integrationLayout);

        auto frequencyRangeLayout = new QHBoxLayout;
        frequencyRangeLayout->addWidget(frequencyRangeLabel);
        frequencyRangeLayout->addWidget(frequencyMinSpinBox);
        frequencyRangeLayout->addWidget(frequencyMaxSpinBox);
        frequencyRangeLayout->addWidget(autoFrequencyRangeCheckBox);
        mainLayout->addLayout(frequencyRangeLayout);

        auto frequencyScaleLayout = new QHBoxLayout;
        frequencyScaleLayout->addWidget(frequencyScaleLabel);
        frequencyScaleLayout->addWidget(frequencyScaleComboBox);
        frequencyScaleLayout->addWidget(frequencyStepLabel);
        frequencyScaleLayout->addWidget(frequencyStepDoubleSpinBox);
        frequencyScaleLayout->addWidget(numberOfPointsFreqLabel);
        frequencyScaleLayout->addWidget(numberOfPointsFreqSpinBox);
        frequencyScaleLayout->addWidget(autoFrequencyPointsCheckBox);
        mainLayout->addLayout(frequencyScaleLayout);

        setLayout(mainLayout);
    }

private slots:
    void onSimulationTimeCheckBoxChanged(int state) {
        simulationTimeSpinBox->setEnabled(state == Qt::Unchecked);
        autoIntegrationCheckBox->setEnabled(state == Qt::Unchecked);
        if (state == Qt::Checked)
            autoIntegrationCheckBox->setCheckState(Qt::Checked);
    }

    void onAutoIntegrationCheckBoxChanged(int state) {
        integrationStepDoubleSpinBox->setEnabled(state == Qt::Unchecked);
        numberOfIntervalsSpinBox->setEnabled(state == Qt::Unchecked);
    }

    void onAutoFrequencyRangeCheckBoxChanged(int state){
        frequencyMinSpinBox->setEnabled(state == Qt::Unchecked);
        frequencyMaxSpinBox->setEnabled(state == Qt::Unchecked);
        autoFrequencyPointsCheckBox->setEnabled(state == Qt::Unchecked);
        if (state == Qt::Checked)
            autoFrequencyPointsCheckBox->setCheckState(Qt::Checked);
    }

    void onAutoFrequencyPointsCheckBoxChanged(int state){
        if (state)
            frequencyScaleComboBox->setCurrentIndex(0);
        frequencyScaleComboBox->setEnabled(state == Qt::Unchecked);
        numberOfPointsFreqSpinBox->setEnabled(state == Qt::Unchecked);
    }

    void onFrequencyScaleChanged(int index){
        frequencyStepDoubleSpinBox->setEnabled(index == 1); //Линейная шкала
    }

    void onSimulationTimeChanged(int value) {
        integrationStepDoubleSpinBox->setMinimum(double(value) / 2000);
        integrationStepDoubleSpinBox->setMaximum(double(value) / 10);
        onNumberOfPointsChanged();
    }

    void onIntegrationStepChanged() {
        const auto time = simulationTimeSpinBox->value();
        const auto dt = integrationStepDoubleSpinBox->value();
        numberOfIntervalsSpinBox->setValue(int(time / dt));
    }

    void onNumberOfPointsChanged() {
        const auto time = simulationTimeSpinBox->value();
        const auto points = numberOfIntervalsSpinBox->value();
        integrationStepDoubleSpinBox->setValue(double(time) / points);
    }
};

#endif //REGVALVE_MOD_PAR_DIALOG_HPP
