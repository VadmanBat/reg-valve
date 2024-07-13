//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* GraphWindow::createNumTab() {
    QLabel *transferFunctionLabel = new QLabel("W<sub>ОУ</sub>(p) = ");
    transferFunctionLabel->setAlignment(Qt::AlignCenter);
    transferFunctionLabel->setStyleSheet("font-size: 24pt;");

    QHBoxLayout *numeratorLayout = new QHBoxLayout;
    QHBoxLayout *denominatorLayout = new QHBoxLayout;

    QDoubleValidator *realNumberValidator = new QDoubleValidator(this);
    realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

    createLineEdit("Числитель:\t", numeratorLayout, realNumberValidator);
    createLineEdit("Знаменатель:\t", denominatorLayout, realNumberValidator);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMidLineWidth(10);

    QVBoxLayout *transferFunctionLayout = new QVBoxLayout();
    transferFunctionLayout->addLayout(numeratorLayout);
    transferFunctionLayout->addWidget(line);
    transferFunctionLayout->addLayout(denominatorLayout);

    QHBoxLayout *uppLayout = new QHBoxLayout;
    uppLayout->addWidget(transferFunctionLabel);
    uppLayout->addLayout(transferFunctionLayout);

    QPushButton *calculateButton = new QPushButton("Рассчитать");
    connect(calculateButton, &QPushButton::clicked, [this, numeratorLayout, denominatorLayout] {
        QVector<double> numeratorData = getLineEditData(numeratorLayout);
        QVector<double> denominatorData = getLineEditData(denominatorLayout);
        //calculateAndPlot(numeratorData, denominatorData);
    });

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(uppLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *numTab = new QWidget();
    numTab->setLayout(resLayout);
    return numTab;
}
