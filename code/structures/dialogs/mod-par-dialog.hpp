//
// Created by Vadim on 13.12.2024.
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

#include "code/structures/model-param.hpp"

class ModParDialog : public QDialog {
Q_OBJECT

private:
    QCheckBox* autoSimTimeCheckBox;
    QCheckBox* autoTimeIntervalsCheckBox;
    QCheckBox* autoFreqRangeCheckBox;
    QCheckBox* autoFreqIntervalsCheckBox;
    QCheckBox* hybridDelayElementCheckBox;

    QSpinBox* simTimeSpinBox;
    QSpinBox* timeIntervalsSpinBox;
    QSpinBox* freqIntervalsSpinBox;
    QSpinBox* approxOrderSpinBox;

    QDoubleSpinBox* freqMinSpinBox;
    QDoubleSpinBox* freqMaxSpinBox;

    QComboBox* freqScaleComboBox;

    static QLabel* createLabel(const QString& text, Qt::AlignmentFlag flag) {
        auto label = new QLabel(text);
        label->setAlignment(flag);
        label->setStyleSheet("padding: 5px;");
        return label;
    };

    QGridLayout* getLayout() {
        auto layout = new QGridLayout;

        auto timeTitleLabel     = createLabel("Временная характеристика", Qt::AlignCenter);
        auto freqTitleLabel     = createLabel("Частотная характеристика", Qt::AlignCenter);
        auto approxTitleLabel   = createLabel("Аппроксимация звена запаздывания через ряд Паде", Qt::AlignCenter);

        timeTitleLabel->setStyleSheet("font-weight: bold; padding: 5px;");
        freqTitleLabel->setStyleSheet("font-weight: bold; padding: 5px;");
        approxTitleLabel->setStyleSheet("font-weight: bold; padding: 5px;");

        auto simTimeLabel       = createLabel("Время:", Qt::AlignRight);
        auto timeIntervalsLabel = createLabel("Интервалы:", Qt::AlignRight);
        auto freqScaleLabel     = createLabel("Шкала:", Qt::AlignRight);
        auto freqMaxLabel       = createLabel("до:", Qt::AlignRight);
        auto freqMinLabel       = createLabel("от:", Qt::AlignRight);
        auto intervalsFreqLabel = createLabel("Интервалы:", Qt::AlignRight);
        auto approxOrderLabel   = createLabel("Порядок разложения:", Qt::AlignRight);

        layout->addWidget(timeTitleLabel, 0, 0, 1, 5);

        layout->addWidget(simTimeLabel, 1, 0);
        layout->addWidget(simTimeSpinBox, 1, 1);
        layout->addWidget(autoSimTimeCheckBox, 1, 4);

        layout->addWidget(timeIntervalsLabel, 2, 0);
        layout->addWidget(timeIntervalsSpinBox, 2, 1);
        layout->addWidget(autoTimeIntervalsCheckBox, 2, 4);

        layout->addWidget(freqTitleLabel, 3, 0, 1, 5);

        layout->addWidget(freqMinLabel, 4, 0);
        layout->addWidget(freqMinSpinBox, 4, 1);
        layout->addWidget(freqMaxLabel, 4, 2);
        layout->addWidget(freqMaxSpinBox, 4, 3);
        layout->addWidget(autoFreqRangeCheckBox, 4, 4);

        layout->addWidget(freqScaleLabel, 5, 0);
        layout->addWidget(freqScaleComboBox, 5, 1);
        layout->addWidget(intervalsFreqLabel, 5, 2);
        layout->addWidget(freqIntervalsSpinBox, 5, 3);
        layout->addWidget(autoFreqIntervalsCheckBox, 5, 4);

        layout->addWidget(approxTitleLabel, 6, 0, 1, 5);

        layout->addWidget(approxOrderLabel, 7, 0);
        layout->addWidget(approxOrderSpinBox, 7, 1);
        layout->addWidget(hybridDelayElementCheckBox, 7, 3, 1, 2);

        freqMinSpinBox->setMinimumWidth(130);
        freqMaxSpinBox->setMinimumWidth(130);

        for (int i = 0; i < 8; ++i)
            layout->setColumnStretch(i, 1);

        return layout;
    }

    void applyStyles() {
        QString commonStyle = R"(
        QWidget {
            font-family: "Helvetica Neue", sans-serif;
            font-size: 12px;
            color: #333333;
        }

        QSpinBox, QDoubleSpinBox, QComboBox {
            padding: 5px;
            border: 1px solid #c0c0c0;
            border-radius: 3px;
            background-color: white;
            color: black;
            selection-background-color: #e0eaf7;
            selection-color: black;
        }

        QSpinBox:hover, QDoubleSpinBox:hover, QComboBox:hover {
            border-color: #909090;
        }

        QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
            border-color: #4682B4;
            outline: none;
        }

        QSpinBox:disabled, QDoubleSpinBox:disabled, QComboBox:disabled {
            background-color: #f0f0f0;
            border: 1px solid #d0d0d0;
            color: #808080;
        }

        QComboBox QAbstractItemView {
            border: 1px solid #c0c0c0;
            background-color: white;
            selection-background-color: #47a3ff;
            color: black;
            outline: none;
        }

        QComboBox QAbstractItemView::item {
            padding: 5px;
        }

        QComboBox QAbstractItemView::item:selected {
             background-color: #47a3ff;
             color: white;
        }

        QCheckBox {
            spacing: 5px;
            font-size: 12px;
            color: #333333;
        }

        QLabel {
            font-size: 12px;
        }
        )";
        setStyleSheet(commonStyle);
    }

public:
    explicit ModParDialog(const ModelParam& values, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Параметры моделирования");
        setWindowIcon(QIcon::fromTheme("document-properties"));

        autoSimTimeCheckBox         = new QCheckBox("Авто");
        autoTimeIntervalsCheckBox   = new QCheckBox("Авто");
        autoFreqRangeCheckBox       = new QCheckBox("Авто");
        autoFreqIntervalsCheckBox   = new QCheckBox("Авто");
        hybridDelayElementCheckBox  = new QCheckBox("Гибридный метод");

        simTimeSpinBox          = new QSpinBox;
        timeIntervalsSpinBox    = new QSpinBox;
        freqIntervalsSpinBox    = new QSpinBox;
        approxOrderSpinBox      = new QSpinBox;

        freqMinSpinBox = new QDoubleSpinBox;
        freqMaxSpinBox = new QDoubleSpinBox;

        freqScaleComboBox = new QComboBox;

        connect(autoSimTimeCheckBox,            &QCheckBox::toggled, this, &ModParDialog::onSimTimeCheckBoxChanged);
        connect(autoTimeIntervalsCheckBox,      &QCheckBox::toggled, this, &ModParDialog::onAutoTimeIntervalsCheckBoxChanged);
        connect(autoFreqRangeCheckBox,          &QCheckBox::toggled, this, &ModParDialog::onAutoFreqRangeCheckBoxChanged);
        connect(autoFreqIntervalsCheckBox,      &QCheckBox::toggled, this, &ModParDialog::onAutoFreqIntervalsCheckBoxChanged);

        if (values.autoSimTime)
            autoSimTimeCheckBox->setCheckState(Qt::Checked);
        else {
            autoSimTimeCheckBox->setCheckState(Qt::Unchecked);
            autoTimeIntervalsCheckBox->setCheckState(values.autoTimeIntervals ? Qt::Checked : Qt::Unchecked);
        }
        autoFreqRangeCheckBox->setCheckState(values.autoFreqRange ? Qt::Checked : Qt::Unchecked);
        autoFreqIntervalsCheckBox->setCheckState(values.autoFreqIntervals ? Qt::Checked : Qt::Unchecked);
        hybridDelayElementCheckBox->setCheckState(values.hybridDelayElement ? Qt::Checked : Qt::Unchecked);

        simTimeSpinBox->setMinimum(10);
        simTimeSpinBox->setMaximum(10000);
        simTimeSpinBox->setValue(values.simTime);
        simTimeSpinBox->setSuffix(" сек");

        timeIntervalsSpinBox->setMinimum(10);
        timeIntervalsSpinBox->setMaximum(2000);
        timeIntervalsSpinBox->setValue(values.timeIntervals);

        freqScaleComboBox->addItem("Логарифмическая");
        freqScaleComboBox->addItem("Линейная");
        freqScaleComboBox->setCurrentIndex(values.freqScale);

        freqMinSpinBox->setMinimum(0.01);
        freqMinSpinBox->setMaximum(200);
        freqMinSpinBox->setValue(values.freqMin);
        freqMinSpinBox->setSuffix(" Гц");
        freqMaxSpinBox->setMinimum(0.01);
        freqMaxSpinBox->setMaximum(200);
        freqMaxSpinBox->setValue(values.freqMax);
        freqMaxSpinBox->setSuffix(" Гц");

        freqIntervalsSpinBox->setMinimum(2);
        freqIntervalsSpinBox->setMaximum(2000);
        freqIntervalsSpinBox->setValue(values.freqIntervals);

        approxOrderSpinBox->setMinimum(1);
        approxOrderSpinBox->setMaximum(6);
        approxOrderSpinBox->setValue(values.approxOrder);

        auto applyButton    = new QPushButton(tr("Применить"));
        auto cancelButton   = new QPushButton(tr("Отменить"));

        connect(applyButton,    &QPushButton::clicked, this, &ModParDialog::accept);
        connect(cancelButton,   &QPushButton::clicked, this, &ModParDialog::reject);

        auto buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(applyButton);
        buttonLayout->addWidget(cancelButton);

        auto formLayout = new QFormLayout(this);
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->addRow(getLayout());
        formLayout->addRow(buttonLayout);
        setLayout(formLayout);

        //applyStyles();
    }

    [[nodiscard]] ModelParam data() const {
        return ModelParam(
                autoSimTimeCheckBox->isChecked(),
                autoTimeIntervalsCheckBox->isChecked(),
                autoFreqRangeCheckBox->isChecked(),
                autoFreqIntervalsCheckBox->isChecked(),
                hybridDelayElementCheckBox->isChecked(),
                simTimeSpinBox->value(),
                timeIntervalsSpinBox->value(),
                freqIntervalsSpinBox->value(),
                approxOrderSpinBox->value(),
                freqMinSpinBox->value(),
                freqMaxSpinBox->value(),
                freqScaleComboBox->currentIndex()
        );
    }

private slots:
    void onSimTimeCheckBoxChanged(bool state) {
        simTimeSpinBox->setEnabled(!state);
        autoTimeIntervalsCheckBox->setEnabled(!state);
        if (state)
            autoTimeIntervalsCheckBox->setCheckState(Qt::Checked);
    }

    void onAutoTimeIntervalsCheckBoxChanged(bool state) {
        timeIntervalsSpinBox->setEnabled(!state);
    }

    void onAutoFreqRangeCheckBoxChanged(bool state) {
        freqMinSpinBox->setEnabled(!state);
        freqMaxSpinBox->setEnabled(!state);
    }

    void onAutoFreqIntervalsCheckBoxChanged(bool state) {
        if (state)
            freqScaleComboBox->setCurrentIndex(0);
        freqScaleComboBox->setEnabled(!state);
        freqIntervalsSpinBox->setEnabled(!state);
    }
};

#endif //REGVALVE_MOD_PAR_DIALOG_HPP
