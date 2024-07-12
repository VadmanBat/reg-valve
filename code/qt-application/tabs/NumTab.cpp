//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

void GraphWindow::createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator) {
    layout->addWidget(new QLabel(name));

    QLineEdit *lineEdit[6];
    for (int i = 0; i < 6; ++i) {
        layout->addWidget(lineEdit[i] = new QLineEdit);
        lineEdit[i]->setAlignment(Qt::AlignRight);
        lineEdit[i]->setStyleSheet("font-size: 16pt;");
        lineEdit[i]->setText("+0");

        if (i > 0) { // Добавление метки степени p, если это не свободный член
            QLabel *label = new QLabel(QString("p<sup>%1</sup>").arg(i));
            label->setStyleSheet("font-size: 16pt;");
            layout->addWidget(label);
        }

        lineEdit[i]->setValidator(validator);
    }
}

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
    line->setMidLineWidth(100);

    QVBoxLayout *transferFunctionLayout = new QVBoxLayout();
    transferFunctionLayout->addLayout(numeratorLayout);
    transferFunctionLayout->addWidget(line);
    transferFunctionLayout->addLayout(denominatorLayout);

    QHBoxLayout *uppLayout = new QHBoxLayout;
    uppLayout->addWidget(transferFunctionLabel);
    uppLayout->addLayout(transferFunctionLayout);

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(uppLayout);
    resLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *numTab = new QWidget();
    numTab->setLayout(resLayout);
    return numTab;
}
