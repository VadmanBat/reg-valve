#include "code/widgets/tran_func_form.h"

#include "code/dialogs/tran_func_dialog.h"
#include "code/util/style_core.hpp"

#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include <QFocusEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QVBoxLayout>

#include <cmath>

class TranFuncForm::LineEdit : public QLineEdit {
public:
    explicit LineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

protected:
    void focusOutEvent(QFocusEvent* event) override {
        QLineEdit::focusOutEvent(event);
        emit editingFinished();
    }
};

TranFuncForm::TranFuncForm(std::size_t n, std::size_t m, const QString& title, QWidget* parent)
    : QWidget(parent), numerator_(n), denominator_(m) {
    auto* titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24pt;");
    createLabelContextMenu(titleLabel);

    auto* validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::StandardNotation);

    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMidLineWidth(10);

    auto* tfLayout = new QVBoxLayout;
    tfLayout->addLayout(createCoeffRow(numerator_, validator, 1));
    tfLayout->addWidget(line);
    tfLayout->addLayout(createCoeffRow(denominator_, validator, 1 << static_cast<int>(n)));

    auto* root = new QHBoxLayout(this);
    root->addWidget(titleLabel);
    root->addLayout(tfLayout);

    auto* delayLabel = new QLabel(QStringLiteral("e"), this);
    delayLabel->setAlignment(Qt::AlignCenter);
    delayLabel->setStyleSheet("font-size: 24pt;");
    root->addWidget(delayLabel);

    delayElement_ = new QDoubleSpinBox(this);
    delayElement_->setDecimals(3);
    delayElement_->setRange(0, 1e6);
    delayElement_->setValue(0);
    auto* delayLayout = new QGridLayout;
    delayLayout->addWidget(delayElement_, 0, 0);
    root->addLayout(delayLayout);
}

QLayout* TranFuncForm::createCoeffRow(VecLine& lineEdits, QDoubleValidator* validator, int factor) {
    auto* layout = new QHBoxLayout;
    int p = -1;
    for (auto& lineEdit : lineEdits) {
        lineEdit = new LineEdit(this);
        layout->addWidget(lineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setStyleSheet("font-size: 16pt; color: violet;");
        lineEdit->setText(QStringLiteral("+0"));

        auto* label = new QLabel(QString("p<sup>%1</sup>").arg(++p), this);
        label->setStyleSheet("font-size: 16pt; color: rgba(0, 0, 0, 64);");
        layout->addWidget(label);

        is_active_.push_back(false);
        const std::size_t index = is_active_.size() - 1;
        lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        lineEdit->setValidator(validator);

        connect(lineEdit, &QLineEdit::textChanged, this, [lineEdit, label] {
            const auto text = lineEdit->text();
            if (auto corrected = correctLine(text); !corrected.isEmpty()) {
                lineEdit->setText(corrected);
                return;
            }
            StyleCore::updateStyle(label, QStringLiteral("color"),
                                   getValue(lineEdit->text()) == 0 ? QStringLiteral("rgba(0, 0, 0, 64)")
                                                                   : QStringLiteral("rgba(0, 0, 0, 255)"));
            adjustLineEditWidth(lineEdit);
            StyleCore::updateStyle(lineEdit, QStringLiteral("color"), getColor(getValue(text)));
        });

        connect(lineEdit, &QLineEdit::editingFinished, this, [this, lineEdit, label, factor, p, index] {
            if (lineEdit->text().isEmpty()) {
                lineEdit->setText(QStringLiteral("+0"));
                StyleCore::updateStyle(label, QStringLiteral("color"), QStringLiteral("rgba(0, 0, 0, 64)"));
            }
            if (getValue(lineEdit->text()) == 0) {
                if (is_active_[index]) {
                    id_ -= factor << p;
                    is_active_[index] = false;
                }
            } else if (!is_active_[index]) {
                id_ += factor << p;
                is_active_[index] = true;
            }
            emit coefficientsChanged();
        });

        lineEdit->setMinimumWidth(36);
        lineEdit->setMaximumWidth(36);
    }
    layout->setAlignment(Qt::AlignLeft);
    return layout;
}

void TranFuncForm::createLabelContextMenu(QLabel* label) {
    auto* aboutAction = new QAction(tr("Подробнее"), label);
    connect(aboutAction, &QAction::triggered, this, [this] {
        if (tf_) {
            TranFuncDialog dialog(*tf_, this);
            dialog.exec();
        }
    });
    auto* menu = new QMenu(label);
    menu->addAction(aboutAction);
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(label, &QLabel::customContextMenuRequested, this, [menu, label](const QPoint& pos) {
        menu->exec(label->mapToGlobal(pos));
    });
}

void TranFuncForm::updateNameLabel() {
    if (nameLabel_)
        nameLabel_->setText(linkName());
}

void TranFuncForm::bindNameLabel(QLabel* label) {
    nameLabel_ = label;
    for (auto* le : numerator_)
        connect(le, &QLineEdit::editingFinished, this, &TranFuncForm::updateNameLabel);
    for (auto* le : denominator_)
        connect(le, &QLineEdit::editingFinished, this, &TranFuncForm::updateNameLabel);
}

void TranFuncForm::setTransferFunction(const numina::TransferFunction* tf) {
    tf_ = tf;
}

QString TranFuncForm::getColor(double value) {
    if (value == 0)
        return QStringLiteral("violet");
    return value > 0 ? QStringLiteral("blue") : QStringLiteral("red");
}

double TranFuncForm::getValue(QString text) {
    text.replace(',', '.');
    if (text.size() == 1)
        text += '1';
    return text.toDouble();
}

void TranFuncForm::adjustLineEditWidth(QLineEdit* lineEdit) {
    const int width = lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 20;
    lineEdit->setMinimumWidth(width);
    lineEdit->setMaximumWidth(width);
}

TranFuncForm::Vec TranFuncForm::reverseOptimize(const Vec& container) {
    auto first = container.rbegin();
    const auto last = container.rend();
    if (first == last)
        return {};
    while (first != last && *first == 0)
        ++first;
    return {first, last};
}

TranFuncForm::Vec TranFuncForm::getLineEditData(const VecLine& lineEdits) {
    const auto size = lineEdits.size();
    Vec values(size);
    for (std::size_t i = 0; i < size; ++i)
        values[i] = getValue(lineEdits[i]->text());
    return reverseOptimize(values);
}

QString TranFuncForm::correctLine(const QString& text) {
    if (text.isEmpty())
        return {};
    if (text.at(0) == ',')
        return QStringLiteral("+0") + text;
    if (text.at(0).isDigit())
        return QChar('+') + text;
    if (text.size() > 1 && text.at(1) == ',')
        return text.at(0) + QStringLiteral("0") + text.mid(1);
    return {};
}

std::vector<double> TranFuncForm::numerator() const {
    return getLineEditData(numerator_);
}

std::vector<double> TranFuncForm::denominator() const {
    return getLineEditData(denominator_);
}

bool TranFuncForm::hasDelay() const {
    return delayElement_->value() != 0;
}

double TranFuncForm::delayTime() const {
    return delayElement_->value();
}

QString TranFuncForm::linkName() const {
    switch (id_) {
    case 1 + 1 * 64:
        return tr("Усилительное (безынерционное)");
    case 1 + 2 * 64:
        return tr("Идеальное интегрирующее (астатическое)");
    case 1 + 3 * 64:
        return tr("Инерционное 1-го порядка (апериодическое)");
    case 1 + 5 * 64:
        return tr("Вырожденное колебательное (консервативное)");
    case 1 + 6 * 64:
        return tr("Реальное интегрирующее (инерционное)");
    case 1 + 7 * 64:
        if (getValue(denominator_[1]->text()) / std::sqrt(getValue(denominator_[2]->text())) / 2 < 1)
            return tr("Колебательное");
        return tr("Инерционное 2-го порядка (апериодическое)");
    case 2 + 1 * 64:
        return tr("Идеальное дифференцирующее");
    case 2 + 3 * 64:
        return tr("Инерционное (реальное) дифференцирующее");
    case 4 + 3 * 64:
        return tr("Реальное дифференцирующее 2-го порядка");
    case 3 + 2 * 64:
        return tr("Изодромное");
    case 3 + 1 * 64:
        return tr("Форсирующее");
    case 3 + 3 * 64:
        return tr("Инерционно-форсирующее");
    case 63 + 21 * 64:
        return tr("Пропорционально-дифференциальное 2-го порядка");
    default:
        return tr("Неизвестно");
    }
}
