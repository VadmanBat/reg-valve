//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

QString Application::getColor(const double& value) {
    if (value == 0)
        return "violet";
    return value > 0 ? "blue" : "red";
}

double Application::getValue(QString text) {
    text.replace(',', '.');
    if (text.count() == 1)
        text += '1';
    return text.toDouble();
}

void Application::adjustLineEditWidth(QLineEdit* lineEdit) {
    int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 10);
    lineEdit->setMinimumWidth(width);
    lineEdit->setMaximumWidth(width);
}

MathCore::Vec Application::getLineEditData(const std::vector <LineEdit*>& lineEdits) {
    const auto count(lineEdits.size());
    MathCore::Vec values(count);
    for (int i = 0; i < count; ++i)
        values[i] = getValue(lineEdits[i]->text());
    return reverseOptimize(values);
}

QString Application::correctLine(const QString& text) {
    if (text.at(0) == ',')
        return "+0" + text;
    if (text.at(0).isDigit())
        return '+' + text;
    if (text.count() > 1 && text.at(1) == ',')
        return text.at(0) + '0' + text.mid(1);
    return "";
}

QLayout* Application::createLineEdit(std::vector <LineEdit*>& lineEdits, QDoubleValidator* validator) {
    auto layout = new QHBoxLayout;
    int p(-1);
    for (auto& lineEdit : lineEdits) {
        layout->addWidget(lineEdit = new LineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText("+0");

        auto label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
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
            StyleCore::updateStyleSheetProperty(lineEdit, "color", getColor(getValue(text)));
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

    return layout;
}

QLayout* Application::createTransferFunctionForm(std::vector <LineEdit*>& numerator, std::vector <LineEdit*>& denominator,
                                                 std::size_t n, std::size_t m, const QString& title) {
    auto transferFunctionLabel = new QLabel(title);
    transferFunctionLabel->setAlignment(Qt::AlignCenter);
    transferFunctionLabel->setStyleSheet("font-size: 24pt;");

    numerator.resize(n);
    denominator.resize(m);
    auto realNumberValidator = new QDoubleValidator;
    realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

    /*auto numLayout = createLineEdit(numerator, realNumberValidator);
    auto denLayout = createLineEdit(denominator, realNumberValidator);*/

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMidLineWidth(10);

    auto transferFunctionLayout = new QVBoxLayout;
    transferFunctionLayout->addLayout(createLineEdit(numerator, realNumberValidator));
    transferFunctionLayout->addWidget(line);
    transferFunctionLayout->addLayout(createLineEdit(denominator, realNumberValidator));

    auto layout = new QHBoxLayout;
    layout->addWidget(transferFunctionLabel);
    layout->addLayout(transferFunctionLayout);

    return layout;
}