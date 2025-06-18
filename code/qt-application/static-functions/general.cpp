//
// Created by Vadim on 21.07.2024.
//
#include "../application.h"
#include <QMessageBox>

void Application::showError(const QString& errorMessage) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);

    msgBox.setText("Произошла ошибка ввода...");
    msgBox.setInformativeText(errorMessage);

    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}