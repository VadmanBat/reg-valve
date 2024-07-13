//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

QString GraphWindow::getColor(const double& value) {
    if (value == 0)
        return "violet";
    return value > 0 ? "blue" : "red";
}

void GraphWindow::adjustLineEditWidth(QLineEdit* lineEdit) {
    int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 10);
    lineEdit->setMinimumWidth(width);
    lineEdit->setMaximumWidth(width);
}

QVector<double> GraphWindow::getLineEditData(QHBoxLayout* layout) {
    QVector<double> data;
    for (int i = 0; i < layout->count(); ++i) {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(layout->itemAt(i)->widget());
        if (lineEdit) {
            data.append(lineEdit->text().toDouble());
        }
    }
    return data;
}

void GraphWindow::updateStyleSheetProperty(QLineEdit* lineEdit, const QString& property, const QString& value) {
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

void GraphWindow::createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator) {
    layout->addWidget(new QLabel(name));

    int p(-1);
    QLineEdit* lineEdits[6];
    for (auto lineEdit : lineEdits) {
        layout->addWidget(lineEdit = new QLineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText("+0");

        QLabel *label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
        label->setStyleSheet("font-size: 16pt;");
        layout->addWidget(label);

        lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(lineEdit, &QLineEdit::textChanged, [lineEdit] {
            auto text(lineEdit->text());
            if (text.isEmpty()) {
                lineEdit->setText("+0");
                return;
            }
            if (lineEdit->text().at(0).isDigit()) {
                lineEdit->setText('+' + lineEdit->text());
                return;
            }

            adjustLineEditWidth(lineEdit);
            text.replace(',', '.');
            updateStyleSheetProperty(lineEdit, "color", getColor(text.toDouble());
        });

        lineEdit->setMinimumWidth(36);
        lineEdit->setMaximumWidth(36);
        lineEdit->setValidator(validator);
    }
    layout->setAlignment(Qt::AlignLeft);
}