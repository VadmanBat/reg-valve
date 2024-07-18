//
// Created by Vadma on 13.07.2024.
//
#include "../application.h"

QString GraphWindow::getColor(const double& value) {
    if (value == 0)
        return "violet";
    return value > 0 ? "blue" : "red";
}

double GraphWindow::getValue(QString text) {
    text.replace(',', '.');
    if (text.count() == 1)
        text += '1';
    return text.toDouble();
}

void GraphWindow::adjustLineEditWidth(QLineEdit* lineEdit) {
    int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 10);
    lineEdit->setMinimumWidth(width);
    lineEdit->setMaximumWidth(width);
}

MathCore::Vec GraphWindow::getLineEditData(QHBoxLayout* layout) {
    MathCore::Vec data;
    data.reserve(6);
    const auto count(layout->count());
    for (int i = 0; i < count; ++i) {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(layout->itemAt(i)->widget());
        if (lineEdit)
            data.emplace_back(getValue(lineEdit->text()));
    }
    return reverseOptimize(data);
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

QString GraphWindow::correctLine(const QString& text) {
    if (text.at(0) == ',')
        return "+0" + text;
    if (text.at(0).isDigit())
        return '+' + text;
    if (text.count() > 1 && text.at(1) == ',')
        return text.at(0) + '0' + text.mid(1);
    return "";
}

void GraphWindow::createLineEdit(const char* name, QHBoxLayout* layout, QDoubleValidator* validator) {
    layout->addWidget(new QLabel(name));

    int p(-1);
    QLineEdit* lineEdits[6];
    for (auto lineEdit : lineEdits) {
        layout->addWidget(lineEdit = new MyLineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText("+0");

        QLabel *label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
        label->setStyleSheet("font-size: 16pt;");
        layout->addWidget(label);

        lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(lineEdit, &QLineEdit::textChanged, [lineEdit] {
            auto text(lineEdit->text());
            if (auto correctedLine(correctLine(text)); !correctedLine.isEmpty()) {
                lineEdit->setText(correctedLine);
                return;
            }
            adjustLineEditWidth(lineEdit);
            updateStyleSheetProperty(lineEdit, "color", getColor(getValue(text)));
        });
        connect(lineEdit, &QLineEdit::editingFinished, [lineEdit] {
            if (lineEdit->text().isEmpty())
                lineEdit->setText("+0");
        });
        lineEdit->setMinimumWidth(36);
        lineEdit->setMaximumWidth(36);
        lineEdit->setValidator(validator);
    }
    layout->setAlignment(Qt::AlignLeft);
}