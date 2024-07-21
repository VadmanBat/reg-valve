//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

void Application::updateStyleSheetProperty(QLineEdit* lineEdit, const QString& property, const QString& value) {
    QString style = lineEdit->styleSheet();
    QString replacement = QString("%1: %2;").arg(property).arg(value);
    if (style.contains(property)) {
        QString pattern = QString("%1: \\w+;").arg(property);
        style = style.replace(QRegExp(pattern), replacement);
    }
    else
        style += replacement;
    lineEdit->setStyleSheet(style);
}

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