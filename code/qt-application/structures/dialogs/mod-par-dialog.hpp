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
    QCheckBox* autoSimTimeCheckBox;
    QCheckBox* autoIntStepCheckBox;
    QCheckBox* autoFreqRangeCheckBox;
    QCheckBox* autoFreqPointsCheckBox;

    QSpinBox* simTimeSpinBox;
    QSpinBox* intIntervalsSpinBox;
    QSpinBox* freqPointsSpinBox;

    QDoubleSpinBox* intStepSpinBox;
    QDoubleSpinBox* freqMinSpinBox;
    QDoubleSpinBox* freqMaxSpinBox;
    QDoubleSpinBox* freqStepSpinBox;

    QComboBox* freqScaleComboBox;

public:
    explicit ModParDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Параметры моделирования");

        auto simTimeLabel       = new QLabel("Время моделирования (с):");
        auto intStepLabel       = new QLabel("Шаг интегрирования (с):");
        auto intIntervalsLabel  = new QLabel("Интервалы:");
        auto freqScaleLabel     = new QLabel("Шкала:");
        auto freqRangeLabel     = new QLabel("Диапазон частот (Гц):");
        auto freqStepLabel      = new QLabel("Шаг (Гц):");
        auto pointsFreqLabel    = new QLabel("Количество точек:");

        autoSimTimeCheckBox     = new QCheckBox("Авто");
        autoIntStepCheckBox     = new QCheckBox("Авто");
        autoFreqRangeCheckBox   = new QCheckBox("Авто");
        autoFreqPointsCheckBox  = new QCheckBox("Авто");

        simTimeSpinBox          = new QSpinBox;
        intIntervalsSpinBox     = new QSpinBox;
        freqPointsSpinBox       = new QSpinBox;

        freqStepSpinBox         = new QDoubleSpinBox;
        intStepSpinBox          = new QDoubleSpinBox;
        freqMinSpinBox          = new QDoubleSpinBox;
        freqMaxSpinBox          = new QDoubleSpinBox;

        freqScaleComboBox       = new QComboBox;

        simTimeSpinBox->setMinimum(10);
        simTimeSpinBox->setMaximum(10000);
        simTimeSpinBox->setValue(500);

        connect(simTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModParDialog::onSimTimeChanged);
        connect(autoSimTimeCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onSimTimeCheckBoxChanged);
        connect(intStepSpinBox, &QDoubleSpinBox::editingFinished, this, &ModParDialog::onIntStepChanged);
        connect(intIntervalsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
                &ModParDialog::onIntIntervalsChanged);
        connect(autoIntStepCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoIntStepCheckBoxChanged);
        connect(autoFreqRangeCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoFreqRangeCheckBoxChanged);
        connect(freqScaleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                &ModParDialog::onFreqScaleChanged);
        connect(autoFreqPointsCheckBox, &QCheckBox::stateChanged, this, &ModParDialog::onAutoFreqPointsCheckBoxChanged);

        autoSimTimeCheckBox->setCheckState(Qt::Checked);
        autoFreqRangeCheckBox->setCheckState(Qt::Checked);

        intStepSpinBox->setMinimum(0.25);
        intStepSpinBox->setMaximum(50);
        intStepSpinBox->setSingleStep(0.1);
        intStepSpinBox->setValue(5);

        intIntervalsSpinBox->setMinimum(10);
        intIntervalsSpinBox->setMaximum(2000);
        intIntervalsSpinBox->setValue(100);

        freqScaleComboBox->addItem("Логарифмическая");
        freqScaleComboBox->addItem("Линейная");

        freqMinSpinBox->setMinimum(0.01);
        freqMinSpinBox->setMaximum(200);
        freqMinSpinBox->setValue(0.1);
        freqMinSpinBox->setPrefix("от: ");
        freqMaxSpinBox->setMinimum(0.01);
        freqMaxSpinBox->setMaximum(200);
        freqMaxSpinBox->setValue(1);
        freqMaxSpinBox->setPrefix("до: ");

        freqStepSpinBox->setMinimum(0.01);
        freqStepSpinBox->setMaximum(2);
        freqStepSpinBox->setValue(0.1);
        freqStepSpinBox->setSingleStep(0.01);

        freqPointsSpinBox->setMinimum(2);
        freqPointsSpinBox->setMaximum(2000);
        freqPointsSpinBox->setValue(100);

        auto mainLayout = new QVBoxLayout;
        auto timeLayout = new QHBoxLayout;
        timeLayout->addWidget(simTimeLabel);
        timeLayout->addWidget(simTimeSpinBox);
        timeLayout->addWidget(autoSimTimeCheckBox);
        mainLayout->addLayout(timeLayout);

        auto integrationLayout = new QHBoxLayout;
        integrationLayout->addWidget(intStepLabel);
        integrationLayout->addWidget(intStepSpinBox);
        integrationLayout->addWidget(intIntervalsLabel);
        integrationLayout->addWidget(intIntervalsSpinBox);
        integrationLayout->addWidget(autoIntStepCheckBox);
        mainLayout->addLayout(integrationLayout);

        auto frequencyRangeLayout = new QHBoxLayout;
        frequencyRangeLayout->addWidget(freqRangeLabel);
        frequencyRangeLayout->addWidget(freqMinSpinBox);
        frequencyRangeLayout->addWidget(freqMaxSpinBox);
        frequencyRangeLayout->addWidget(autoFreqRangeCheckBox);
        mainLayout->addLayout(frequencyRangeLayout);

        auto frequencyScaleLayout = new QHBoxLayout;
        frequencyScaleLayout->addWidget(freqScaleLabel);
        frequencyScaleLayout->addWidget(freqScaleComboBox);
        frequencyScaleLayout->addWidget(freqStepLabel);
        frequencyScaleLayout->addWidget(freqStepSpinBox);
        frequencyScaleLayout->addWidget(pointsFreqLabel);
        frequencyScaleLayout->addWidget(freqPointsSpinBox);
        frequencyScaleLayout->addWidget(autoFreqPointsCheckBox);
        mainLayout->addLayout(frequencyScaleLayout);

        setLayout(mainLayout);
    }

private slots:
    void onSimTimeCheckBoxChanged(int state) {
        simTimeSpinBox->setEnabled(state == Qt::Unchecked);
        autoIntStepCheckBox->setEnabled(state == Qt::Unchecked);
        if (state == Qt::Checked)
            autoIntStepCheckBox->setCheckState(Qt::Checked);
    }

    void onAutoIntStepCheckBoxChanged(int state) {
        intStepSpinBox->setEnabled(state == Qt::Unchecked);
        intIntervalsSpinBox->setEnabled(state == Qt::Unchecked);
    }

    void onAutoFreqRangeCheckBoxChanged(int state){
        freqMinSpinBox->setEnabled(state == Qt::Unchecked);
        freqMaxSpinBox->setEnabled(state == Qt::Unchecked);
        autoFreqPointsCheckBox->setEnabled(state == Qt::Unchecked);
        if (state == Qt::Checked)
            autoFreqPointsCheckBox->setCheckState(Qt::Checked);
    }

    void onAutoFreqPointsCheckBoxChanged(int state){
        if (state)
            freqScaleComboBox->setCurrentIndex(0);
        freqScaleComboBox->setEnabled(state == Qt::Unchecked);
        freqPointsSpinBox->setEnabled(state == Qt::Unchecked);
    }

    void onFreqScaleChanged(int index){
        freqStepSpinBox->setEnabled(index == 1); //Линейная шкала
    }

    void onSimTimeChanged(int value) {
        intStepSpinBox->setMinimum(double(value) / 2000);
        intStepSpinBox->setMaximum(double(value) / 10);
        onIntIntervalsChanged();
    }

    void onIntStepChanged() {
        const auto time = simTimeSpinBox->value();
        const auto dt = intStepSpinBox->value();
        intIntervalsSpinBox->setValue(int(time / dt));
    }

    void onIntIntervalsChanged() {
        const auto time = simTimeSpinBox->value();
        const auto points = intIntervalsSpinBox->value();
        intStepSpinBox->setValue(double(time) / points);
    }
};

#endif //REGVALVE_MOD_PAR_DIALOG_HPP
