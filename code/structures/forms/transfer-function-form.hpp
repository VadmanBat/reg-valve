//
// Created by Vadma on 30.12.2024.
//

#ifndef REGVALVE_TRANSFER_FUNCTION_FORM_HPP
#define REGVALVE_TRANSFER_FUNCTION_FORM_HPP

#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QObject>

#include <QLineEdit>
#include <QFocusEvent>

#include "../../style-core.hpp"

class TransferFunctionForm : public QObject {
private:
    class LineEdit : public QLineEdit {
    public:
        explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

    protected:
        void focusOutEvent(QFocusEvent *event) override {
            QLineEdit::focusOutEvent(event);
            emit editingFinished(); // Явно вызываем сигнал editingFinished
        }
    };

    using VecLine   = std::vector <LineEdit*>;
    using Vec       = std::vector <double>;

    VecLine numerator, denominator;
    QHBoxLayout* mainLayout;

    static QString getColor(const double& value) {
        if (value == 0)
            return "violet";
        return value > 0 ? "blue" : "red";
    }

    static double getValue(QString text) {
        text.replace(',', '.');
        if (text.size() == 1)
            text += '1';
        return text.toDouble();
    }

    static void adjustLineEditWidth(QLineEdit* lineEdit) {
        int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 10);
        lineEdit->setMinimumWidth(width);
        lineEdit->setMaximumWidth(width);
    }

    template <class Container>
    static Container reverseOptimize(const Container& container) {
        auto first(container.rbegin());
        const auto last(container.rend());
        if (first == last)
            return Container();
        while (first != last && *first == 0)
            ++first;
        return Container(first, last);
    }

    static Vec getLineEditData(const VecLine& lineEdits) {
        const auto size = lineEdits.size();
        Vec values(size);
        for (std::size_t i = 0; i < size; ++i)
            values[i] = getValue(lineEdits[i]->text());
        return reverseOptimize(values);
    }

    static QString correctLine(const QString& text) {
        if (text.at(0) == ',')
            return "+0" + text;
        if (text.at(0).isDigit())
            return '+' + text;
        if (text.size() > 1 && text.at(1) == ',')
            return text.at(0) + '0' + text.mid(1);
        return "";
    }

    static QLayout* createLineEdit(VecLine& lineEdits, QDoubleValidator* validator) {
        auto layout = new QHBoxLayout;
        int p(-1);
        for (auto& lineEdit : lineEdits) {
            layout->addWidget(lineEdit = new LineEdit);
            lineEdit->setAlignment(Qt::AlignRight);
            lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
            lineEdit->setText("+0");

            auto label = new QLabel(QString("p<sup>%1</sup>").arg(++p));
            label->setStyleSheet("font-size: 16pt; color: rgba(0, 0, 0, 64);");
            layout->addWidget(label);

            lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            connect(lineEdit, &QLineEdit::textChanged, [lineEdit, label] {
                auto text= lineEdit->text();
                if (auto correctedLine= correctLine(text); !correctedLine.isEmpty()) {
                    lineEdit->setText(correctedLine);
                    return;
                }
                StyleCore::updateStyle(
                        label, "color",
                        getValue(lineEdit->text()) == 0 ? "rgba(0, 0, 0, 64)" : "rgba(0, 0, 0, 255)"
                );
                adjustLineEditWidth(lineEdit);
                StyleCore::updateStyle(lineEdit, "color", getColor(getValue(text)));
            });
            connect(lineEdit, &QLineEdit::editingFinished, [lineEdit, label] {
                if (lineEdit->text().isEmpty()) {
                    lineEdit->setText("+0");
                    StyleCore::updateStyle(label, "color", "rgba(0, 0, 0, 64)");
                }
            });
            lineEdit->setMinimumWidth(36);
            lineEdit->setMaximumWidth(36);
            lineEdit->setValidator(validator);
        }
        layout->setAlignment(Qt::AlignLeft);

        return layout;
    }

public:
    explicit TransferFunctionForm(std::size_t n = 6, std::size_t m = 6, const QString& title = "W(p) = ") :
        numerator(n), denominator(m)
    {
        auto transferFunctionLabel = new QLabel(title);
        transferFunctionLabel->setAlignment(Qt::AlignCenter);
        transferFunctionLabel->setStyleSheet("font-size: 24pt;");

        auto realNumberValidator = new QDoubleValidator;
        realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setMidLineWidth(10);

        auto transferFunctionLayout = new QVBoxLayout;
        transferFunctionLayout->addLayout(createLineEdit(numerator, realNumberValidator));
        transferFunctionLayout->addWidget(line);
        transferFunctionLayout->addLayout(createLineEdit(denominator, realNumberValidator));

        mainLayout = new QHBoxLayout;
        mainLayout->addWidget(transferFunctionLabel);
        mainLayout->addLayout(transferFunctionLayout);
    }

    [[nodiscard]] inline QLayout* getLayout() const {
        return mainLayout;
    }

    [[nodiscard]] inline Vec getNum() const {
        return getLineEditData(numerator);
    }

    [[nodiscard]] inline Vec getDen() const {
        return getLineEditData(denominator);
    }
};

#endif //REGVALVE_TRANSFER_FUNCTION_FORM_HPP
