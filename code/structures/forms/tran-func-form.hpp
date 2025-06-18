//
// Created by Vadim on 30.12.2024.
//

#ifndef REGVALVE_TRAN_FUNC_FORM_HPP
#define REGVALVE_TRAN_FUNC_FORM_HPP

#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QObject>

#include <QLineEdit>
#include <QFocusEvent>

#include "code/structures/dialogs/tran-func-dialog.hpp"
#include "code/transfer-function/transfer-function.hpp"
#include "../../style-core.hpp"

class TranFuncForm : public QObject {
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
    QDoubleSpinBox* delayElement;
    std::vector <bool> is_active;
    QLabel* nameLabel;
    int id;

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
        int width(lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 20);
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

    QLayout* createLineEdit(VecLine& lineEdits, QDoubleValidator* validator, int factor) {
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

            is_active.push_back(false); std::size_t index = is_active.size() - 1;
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
            connect(lineEdit, &QLineEdit::editingFinished, [this, lineEdit, label, factor, p, index] {
                if (lineEdit->text().isEmpty()) {
                    lineEdit->setText("+0");
                    StyleCore::updateStyle(label, "color", "rgba(0, 0, 0, 64)");
                }
                if (getValue(lineEdit->text()) == 0) {
                    if (is_active[index]) {
                        id -= factor << p;
                        is_active[index] = false;
                    }
                }
                else {
                    if (!is_active[index]) {
                        id += factor << p;
                        is_active[index] = true;
                    }
                }
            });
            lineEdit->setMinimumWidth(36);
            lineEdit->setMaximumWidth(36);
            lineEdit->setValidator(validator);
        }
        layout->setAlignment(Qt::AlignLeft);

        return layout;
    }

    void createLabelContextMenu(QLabel* label) {
        QIcon aboutIcon = QIcon::fromTheme("dialog-information");
        QIcon copyIcon  = QIcon::fromTheme("edit-copy");
        QIcon pasteIcon = QIcon::fromTheme("edit-paste");

        auto aboutAction    = new QAction(aboutIcon, tr("Подробнее"), label);
        auto copyAction     = new QAction(copyIcon, tr("Копировать"), label);
        auto insertAction   = new QAction(pasteIcon, tr("Вставить"), label);

        /*connect(aboutAction, &QAction::triggered, [chart, chartView]() {
            QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Сохранить график"), chart->title(), tr("Рисунок в формате PNG (*.png);;Все файлы (*)"));
            if (!fileName.isEmpty()) {
                QPixmap pixmap = chartView->grab();
                if (!pixmap.save(fileName, "png"))
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Не удалось сохранить график!"));
            }
        });

        connect(insertAction, &QAction::triggered, [chart]() {
            QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Сохранить график"), chart->title(), tr("Текст в формате txt (*.txt);;Все файлы (*)"));
            if (!fileName.isEmpty()) {
                if (!saveChartToFile(fileName, chart))
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Не удалось сохранить график!"));
            }
        });

        connect(copyAction, &QAction::triggered, [chartView]() {
            QPixmap pixmap = chartView->grab();
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setImage(pixmap.toImage());
        });*/

        connect(aboutAction, &QAction::triggered, [this]() {
            TranFuncDialog(*tf).exec();
        });

        auto contextMenu = new QMenu(label);

        contextMenu->addAction(aboutAction);
        contextMenu->addSeparator();
        contextMenu->addAction(copyAction);
        contextMenu->addAction(insertAction);

        label->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(label, &QLabel::customContextMenuRequested, [contextMenu, label](const QPoint& pos) {
            contextMenu->exec(label->mapToGlobal(pos));
        });
    }

private slots:
    void updateNameLabel() {
        nameLabel->setText(getName());
    };

public:
    explicit TranFuncForm(std::size_t n = 6, std::size_t m = 6, const QString& title = "W(p) = ") :
        numerator(n), denominator(m), id(0)
    {
        auto transferFunctionLabel = new QLabel(title);
        transferFunctionLabel->setAlignment(Qt::AlignCenter);
        transferFunctionLabel->setStyleSheet("font-size: 24pt;");
        createLabelContextMenu(transferFunctionLabel);

        auto realNumberValidator = new QDoubleValidator;
        realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setMidLineWidth(10);

        auto transferFunctionLayout = new QVBoxLayout;
        transferFunctionLayout->addLayout(createLineEdit(numerator, realNumberValidator, 1));
        transferFunctionLayout->addWidget(line);
        transferFunctionLayout->addLayout(createLineEdit(denominator, realNumberValidator, 1 << n));

        mainLayout = new QHBoxLayout;
        mainLayout->addWidget(transferFunctionLabel);
        mainLayout->addLayout(transferFunctionLayout);

        auto delayElementLabel = new QLabel("e");
        delayElementLabel->setAlignment(Qt::AlignCenter);
        delayElementLabel->setStyleSheet("font-size: 24pt;");
        mainLayout->addWidget(delayElementLabel);
        auto delayElementLayout = new QGridLayout;
        delayElement = new QDoubleSpinBox;
        delayElementLayout->addWidget(delayElement, 0, 0);
        /*delayElementLayout->setRowStretch(0, 1);
        delayElementLayout->setRowStretch(1, 2);*/
        mainLayout->addLayout(delayElementLayout);
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

    [[nodiscard]] inline bool isDelayElement() const {
        return delayElement->value() != 0;
    }

    [[nodiscard]] inline double delayTime() const {
        return delayElement->value();
    }

    void createConnect(QLabel* label) {
        nameLabel = label;
        for (auto& lineEdit : numerator)
            connect(lineEdit, &QLineEdit::editingFinished, this, &TranFuncForm::updateNameLabel);
        for (auto& lineEdit : denominator)
            connect(lineEdit, &QLineEdit::editingFinished, this, &TranFuncForm::updateNameLabel);
    }

    const TransferFunction* tf;
    void setTF(const TransferFunction& tf) {
        this->tf = &tf;
    }

    [[nodiscard]] QString getName() const {
        switch (id) {
            case 1 + 1 * 64:
                return "Усилительное (безынерционное)";
            case 1 + 2 * 64:
                return "Идеальное интегрирующее (астатическое)";
            case 1 + 3 * 64:
                return "Инерционное 1-го порядка (аппериодическое)";
            case 1 + 5 * 64:
                return "Вырожденное колебательное (консервативное)";
            case 1 + 6 * 64:
                return "Реальное интегрирующее (инерционное)";
            case 1 + 7 * 64:
                if (getValue(denominator[1]->text()) / std::sqrt(getValue(denominator[2]->text())) / 2 < 1)
                    return "Колебательное";
                else
                    return "Инерционное 2-го порядка (аппериодическое)";
            case 2 + 1 * 64:
                return "Идеальное дифференцирующее";
            case 2 + 3 * 64:
                return "Инерционное (реальное) дифференцирующее";
            case 4 + 3 * 64:
                return "Реальное дифференцирующее 2-го порядка";
            case 3 + 2 * 64:
                return "Изодромное";
            case 3 + 1 * 64:
                return "Форсирующее";
            case 3 + 3 * 64:
                return "Инерционно-форсирующее";
            case 63 + 21 * 64:
                return "Пропорционально-дифференциальное 2-го порядка";
            default:
                return "Неизвестно";
        }
    }
};

#endif //REGVALVE_TRAN_FUNC_FORM_HPP
