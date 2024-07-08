//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* GraphWindow::createNumTab() {
    QWidget *coefficientsTab = new QWidget();
    QVBoxLayout *coefficientsLayout = new QVBoxLayout(coefficientsTab);

    // Горизонтальный макет для коэффициентов числителя
    QHBoxLayout *numeratorLayout = new QHBoxLayout;
    QLabel *numeratorLabel = new QLabel("Числитель: ");
    numeratorLayout->addWidget(numeratorLabel);

    QLineEdit *a[6];
    for (int i = 0; i < 6; ++i) {
        a[i] = new QLineEdit;
        a[i]->setValidator(new QIntValidator(a[i])); // Валидатор для целых чисел
        numeratorLayout->addWidget(a[i]);
        if (i > 0) { // Добавление метки степени p, если это не свободный член
            QLabel *powerLabel = new QLabel(QString("p^%1").arg(i));
            numeratorLayout->addWidget(powerLabel);
        }
    }

    // Горизонтальный макет для коэффициентов знаменателя
    QHBoxLayout *denominatorLayout = new QHBoxLayout;
    QLabel *denominatorLabel = new QLabel("Знаменатель: ");
    denominatorLayout->addWidget(denominatorLabel);

    QLineEdit *b[6];
    for (int i = 0; i < 6; ++i) {
        b[i] = new QLineEdit;
        b[i]->setValidator(new QIntValidator(b[i])); // Валидатор для целых чисел
        denominatorLayout->addWidget(b[i]);
        if (i > 0) { // Добавление метки степени p, если это не свободный член
            QLabel *powerLabel = new QLabel(QString("p^%1").arg(i));
            denominatorLayout->addWidget(powerLabel);
        }
    }

    // Добавление макетов в общий макет
    coefficientsLayout->addLayout(numeratorLayout);
    coefficientsLayout->addLayout(denominatorLayout);
    coefficientsTab->setLayout(coefficientsLayout);

    return coefficientsTab;
}