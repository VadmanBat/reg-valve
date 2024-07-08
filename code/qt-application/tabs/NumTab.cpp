//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* GraphWindow::createNumTab() {
    QWidget *numTab = new QWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel *label = new QLabel("W<sub>ОУ</sub>(p) = ");
    label->setAlignment(Qt::AlignCenter);
    setFontSize(label, 24);

    QVBoxLayout *coefficientsLayout = new QVBoxLayout();

    QHBoxLayout *numeratorLayout = new QHBoxLayout;
    QHBoxLayout *denominatorLayout = new QHBoxLayout;
    QLabel *numeratorLabel = new QLabel("Числитель:\t");
    QLabel *denominatorLabel = new QLabel("Знаменатель:\t");
    numeratorLayout->addWidget(numeratorLabel);
    denominatorLayout->addWidget(denominatorLabel);

    QLineEdit *a[6], *b[6];
    for (int i = 0; i < 6; ++i) {
        a[i] = new QLineEdit;
        b[i] = new QLineEdit;
        numeratorLayout->addWidget(a[i]);
        denominatorLayout->addWidget(b[i]);
        if (i > 0) { // Добавление метки степени p, если это не свободный член
            QLabel *label = new QLabel(QString("p<sup>%1</sup>").arg(i));
            setFontSize(label, 16);
            numeratorLayout->addWidget(label);
            label = new QLabel(QString("p<sup>%1</sup>").arg(i));
            setFontSize(label, 16);
            denominatorLayout->addWidget(label);
        }
    }

    QDoubleValidator *realNumberValidator = new QDoubleValidator(this);
    realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

    for (int i = 0; i < 6; ++i) {
        a[i]->setValidator(realNumberValidator);
        b[i]->setValidator(realNumberValidator);

        a[i]->setMinimumSize(QSize(10, 0)); // Минимальный размер
        a[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Расширение при необходимости
        b[i]->setMinimumSize(QSize(10, 0));
        b[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        a[i]->setMinimumSize(QSize(50, 50));
        b[i]->setMinimumSize(QSize(50, 50));
    }

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // Добавление макетов в общий макет
    coefficientsLayout->addLayout(numeratorLayout);
    coefficientsLayout->addWidget(line);
    coefficientsLayout->addLayout(denominatorLayout);

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    coefficientsLayout->addItem(verticalSpacer);

    mainLayout->addWidget(label);
    mainLayout->addLayout(coefficientsLayout);

    numTab->setLayout(mainLayout);
    return numTab;
}