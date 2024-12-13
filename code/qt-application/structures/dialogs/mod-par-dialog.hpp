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
    QSpinBox *numberOfPointsSpinBox;
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
        auto numberOfPointsLabel        = new QLabel("Количество точек:");
        auto frequencyScaleLabel        = new QLabel("Шкала:");
        auto frequencyRangeLabel        = new QLabel("Диапазон частот (Гц):");
        auto frequencyStepLabel         = new QLabel("Шаг (Гц):");
        auto numberOfPointsFreqLabel    = new QLabel("Количество точек:");

        simulationTimeSpinBox           = new QSpinBox;
        integrationStepDoubleSpinBox    = new QDoubleSpinBox;
        simulationTimeCheckBox          = new QCheckBox("Авто");
        numberOfPointsSpinBox           = new QSpinBox;
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
        simulationTimeSpinBox->setValue(600);

        connect(simulationTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](){});
        connect(simulationTimeCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onSimulationTimeCheckBoxChanged);
        connect(integrationStepDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ModParDialog::onIntegrationStepChanged);
        connect(numberOfPointsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModParDialog::onNumberOfPointsChanged);
        connect(autoIntegrationCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoIntegrationCheckBoxChanged);
        connect(autoFrequencyRangeCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoFrequencyRangeCheckBoxChanged);
        connect(frequencyScaleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModParDialog::onFrequencyScaleChanged);
        connect(autoFrequencyPointsCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoFrequencyPointsCheckBoxChanged);

        integrationStepDoubleSpinBox->setMinimum(0.1);
        integrationStepDoubleSpinBox->setMaximum(10);
        integrationStepDoubleSpinBox->setSingleStep(0.1);
        integrationStepDoubleSpinBox->setValue(1);

        numberOfPointsSpinBox->setMinimum(10);
        numberOfPointsSpinBox->setMaximum(2000);
        numberOfPointsSpinBox->setValue(100);

        frequencyScaleComboBox->addItem("Логарифмическая");
        frequencyScaleComboBox->addItem("Линейная");

        frequencyMinSpinBox->setMinimum(0.01);
        frequencyMinSpinBox->setMaximum(200);
        frequencyMinSpinBox->setValue(0.1);
        frequencyMaxSpinBox->setMinimum(0.01);
        frequencyMaxSpinBox->setMaximum(200);
        frequencyMaxSpinBox->setValue(1);

        frequencyStepDoubleSpinBox->setEnabled(false);
        frequencyStepDoubleSpinBox->setMinimum(0.01);
        frequencyStepDoubleSpinBox->setMaximum(2);
        frequencyStepDoubleSpinBox->setValue(0.1);
        frequencyStepDoubleSpinBox->setSingleStep(0.01);

        numberOfPointsFreqSpinBox->setMinimum(2);
        numberOfPointsFreqSpinBox->setMaximum(2000);

        auto mainLayout = new QVBoxLayout;
        auto timeLayout = new QHBoxLayout;
        timeLayout->addWidget(simulationTimeLabel);
        timeLayout->addWidget(simulationTimeSpinBox);
        timeLayout->addWidget(simulationTimeCheckBox);
        mainLayout->addLayout(timeLayout);

        auto integrationLayout = new QHBoxLayout;
        integrationLayout->addWidget(integrationStepLabel);
        integrationLayout->addWidget(integrationStepDoubleSpinBox);
        integrationLayout->addWidget(numberOfPointsLabel);
        integrationLayout->addWidget(numberOfPointsSpinBox);
        integrationLayout->addWidget(autoIntegrationCheckBox);
        mainLayout->addLayout(integrationLayout);

        auto frequencyRangeLayout = new QHBoxLayout;
        frequencyRangeLayout->addWidget(frequencyRangeLabel);
        frequencyRangeLayout->addWidget(new QLabel("от:"));
        frequencyRangeLayout->addWidget(frequencyMinSpinBox);
        frequencyRangeLayout->addWidget(new QLabel("до:"));
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
        integrationStepDoubleSpinBox->setEnabled(state == Qt::Unchecked);
        numberOfPointsSpinBox->setEnabled(state == Qt::Unchecked);
        autoIntegrationCheckBox->setEnabled(state == Qt::Unchecked);
    }

    void onAutoIntegrationCheckBoxChanged(int state) {
        integrationStepDoubleSpinBox->setEnabled(state == Qt::Unchecked);
        numberOfPointsSpinBox->setEnabled(state == Qt::Unchecked);
    }

    void onIntegrationStepChanged(double value) {
        if (simulationTimeSpinBox->value() > 0) {
            numberOfPointsSpinBox->setValue(static_cast<int>(std::ceil(simulationTimeSpinBox->value() / value)) +1);
        }
    }

    void onNumberOfPointsChanged(int value) {
        if (simulationTimeSpinBox->value() > 0 && value > 1) {
            integrationStepDoubleSpinBox->setValue(static_cast<double>(simulationTimeSpinBox->value()) / (value - 1));
        }
    }

    void onAutoFrequencyRangeCheckBoxChanged(int state){
        frequencyMinSpinBox->setEnabled(state == Qt::Unchecked);
        frequencyMaxSpinBox->setEnabled(state == Qt::Unchecked);
        frequencyScaleComboBox->setEnabled(state == Qt::Unchecked);
        frequencyStepDoubleSpinBox->setEnabled(state == Qt::Unchecked && frequencyScaleComboBox->currentIndex() == 1);
        numberOfPointsFreqSpinBox->setEnabled(state == Qt::Unchecked);
        autoFrequencyPointsCheckBox->setEnabled(state == Qt::Unchecked);
    }

    void onFrequencyScaleChanged(int index){
        frequencyStepDoubleSpinBox->setEnabled(index == 1); //Линейная шкала
    }

    void onAutoFrequencyPointsCheckBoxChanged(int state){
        numberOfPointsFreqSpinBox->setEnabled(state == Qt::Unchecked);
    }
};

#endif //REGVALVE_MOD_PAR_DIALOG_HPP
