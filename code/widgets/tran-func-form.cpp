#include "code/widgets/tran-func-form.h"

#include "code/dialogs/tran-func-dialog.h"
#include "code/util/format.hxx"
#include "code/util/style-core.hpp"

#include <QAbstractSpinBox>
#include <QApplication>
#include <QClipboard>
#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QShowEvent>
#include <QStyle>
#include <QTimer>
#include <QStyleOptionSpinBox>
#include <QToolTip>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

class TranFuncForm::LineEdit : public QLineEdit {
public:
    explicit LineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

    /// Canonical value: up to 16 significant digits (17th rounded). Never from 6-digit display.
    double stored{0.0};
    /// True only after real user typing (textEdited), not after our display refresh.
    bool dirty{false};
    /// User cleared the field during this edit (even if QValidator later restores text).
    bool cleared{false};

    void setStored(double v) {
        stored = num_format::roundSignificant(v, num_format::STORED_DIGITS);
        showDisplay();
    }

    void showDisplay() {
        const QString disp = num_format::formatCoeff(stored, num_format::SIGNIFICANT_DIGITS, true);
        blockSignals(true);
        setText(disp);
        blockSignals(false);
        dirty   = false;
        cleared = false;
    }

    void showStoredForEdit() {
        // Full stored precision for editing (not the short display form).
        const QString full = num_format::formatCoeff(stored, num_format::STORED_DIGITS, true);
        blockSignals(true);
        setText(full);
        blockSignals(false);
        dirty   = false;
        cleared = false;
    }

    /// Commit editor text into stored only if the user actually typed.
    void commitIfDirty() {
        if (!dirty)
            return;
        // Empty / lone sign / explicit clear → zero (do not keep previous stored).
        const QString t = text().trimmed();
        if (cleared || t.isEmpty() || t == QLatin1String("+") || t == QLatin1String("-")) {
            stored = 0.0;
        } else {
            bool ok = false;
            const double v = num_format::parse(t, &ok);
            if (ok)
                stored = num_format::roundSignificant(v, num_format::STORED_DIGITS);
            else
                stored = 0.0; // unparseable input → 0
        }
        dirty   = false;
        cleared = false;
    }

protected:
    void focusInEvent(QFocusEvent* event) override {
        QLineEdit::focusInEvent(event);
        showStoredForEdit();
        selectAll();
    }
    // Do NOT re-emit editingFinished here: QLineEdit already emits it on focus-out / Enter.
};

namespace {

void applyCoeffSign(QLineEdit* lineEdit, double value) {
    const char* sign = "zero";
    if (value > 0)
        sign = "positive";
    else if (value < 0)
        sign = "negative";
    style_util::setProperty(lineEdit, "coeffSign", QByteArray(sign));
}

void applyPowerActive(QLabel* label, bool active) {
    style_util::setProperty(label, "powerActive", active ? QByteArray("true") : QByteArray("false"));
}

std::vector<double> parseCoeffList(QString line) {
    line.replace(',', ' ');
    line.replace(';', ' ');
    std::vector<double> out;
    const auto parts = line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    for (const QString& p : parts) {
        bool ok = false;
        const double v = num_format::parse(p, &ok);
        if (ok)
            out.push_back(v);
    }
    return out;
}

} // namespace

TranFuncForm::TranFuncForm(std::size_t n, std::size_t m, const QString& title, QWidget* parent)
    : QWidget(parent), numerator_(n), denominator_(m) {
    setObjectName(QStringLiteral("TranFuncForm"));

    auto* titleLabel = new QLabel(title, this);
    titleLabel->setObjectName(QStringLiteral("tfTitle"));
    titleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    createLabelContextMenu(titleLabel);

    auto* validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::ScientificNotation);

    auto* divider = new QFrame(this);
    divider->setObjectName(QStringLiteral("tfDivider"));
    divider->setFrameShape(QFrame::NoFrame);
    divider->setFixedHeight(2);
    divider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    divider->setMinimumWidth(120);
    divider->setAttribute(Qt::WA_StyledBackground, true);

    auto* fracGrid = new QGridLayout;
    fracGrid->setHorizontalSpacing(8);
    fracGrid->setVerticalSpacing(6);
    fracGrid->setContentsMargins(0, 0, 0, 0);
    fracGrid->addWidget(titleLabel, 0, 0, 3, 1, Qt::AlignRight | Qt::AlignVCenter);
    fracGrid->addLayout(createCoeffRow(numerator_, validator, 1), 0, 1);
    fracGrid->addWidget(divider, 1, 1);
    fracGrid->addLayout(createCoeffRow(denominator_, validator, 1 << static_cast<int>(n)), 2, 1);
    fracGrid->setColumnStretch(1, 1);
    fracGrid->setRowMinimumHeight(1, 2);

    auto* root = new QHBoxLayout(this);
    root->setSpacing(8);
    root->setAlignment(Qt::AlignVCenter);
    root->addLayout(fracGrid, 1);

    auto* mulDot = new QLabel(QString::fromUtf8("·"), this);
    mulDot->setObjectName(QStringLiteral("tfMulDot"));
    mulDot->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    root->addWidget(mulDot, 0, Qt::AlignVCenter);

    // Delay: only e^(−τ p) in layout (original vertical position).
    // Copy/paste are overlays — do not take layout space / do not lift the exponent.
    delayGroup_ = new QWidget(this);
    delayGroup_->setObjectName(QStringLiteral("tfDelayGroup"));
    delayGroup_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    auto* delayRoot = new QHBoxLayout(delayGroup_);
    delayRoot->setContentsMargins(0, 0, 0, 0);
    delayRoot->setSpacing(2);

    auto* eBase = new QLabel(QStringLiteral("e"), delayGroup_);
    eBase->setObjectName(QStringLiteral("tfDelayBase"));
    eBase->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    auto* expGroup = new QWidget(delayGroup_);
    expGroup->setObjectName(QStringLiteral("tfDelayExp"));
    auto* expLayout = new QHBoxLayout(expGroup);
    expLayout->setContentsMargins(0, 0, 0, 8);
    expLayout->setSpacing(2);

    auto* minusLab = new QLabel(QString::fromUtf8("−"), expGroup);
    minusLab->setObjectName(QStringLiteral("tfDelayMinus"));
    minusLab->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    delayElement_ = new QDoubleSpinBox(expGroup);
    delayElement_->setObjectName(QStringLiteral("tfDelay"));
    delayElement_->setDecimals(2);
    delayElement_->setSingleStep(0.01);
    delayElement_->setRange(0.0, 30.0);
    delayElement_->setValue(0.0);
    delayElement_->setKeyboardTracking(false);
    delayElement_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    delayElement_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    delayElement_->setToolTip(tr("Постоянная запаздывания τ, с (0…30). Ввод с клавиатуры."));
    delayElement_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    {
        QStyleOptionSpinBox opt;
        opt.initFrom(delayElement_);
        opt.frame = true;
        opt.buttonSymbols = QAbstractSpinBox::NoButtons;
        opt.stepEnabled = QAbstractSpinBox::StepNone;
        const QFontMetrics fm(delayElement_->font());
        const int extra = 2 * fm.horizontalAdvance(QLatin1Char('0'));
        const int textW = fm.horizontalAdvance(QStringLiteral("30.00")) + extra;
        const QSize textSize(textW + 4, fm.height());
        const QSize sz =
            delayElement_->style()->sizeFromContents(QStyle::CT_SpinBox, &opt, textSize, delayElement_);
        delayElement_->setFixedSize(sz.expandedTo(textSize + QSize(14, 6)));
    }

    auto* pLab = new QLabel(QStringLiteral("p"), expGroup);
    pLab->setObjectName(QStringLiteral("tfDelayP"));
    pLab->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    expLayout->addWidget(minusLab);
    expLayout->addWidget(delayElement_);
    expLayout->addWidget(pLab);

    delayRoot->addWidget(eBase, 0, Qt::AlignBottom);
    delayRoot->addWidget(expGroup, 0, Qt::AlignTop);

    root->addWidget(delayGroup_, 0, Qt::AlignVCenter);

    // Overlay buttons (not in layout) — drawn under exp, bottom-right of delay block.
    copyBtn_ = new QPushButton(QStringLiteral("📋"), this);
    copyBtn_->setObjectName(QStringLiteral("tfCopyButton"));
    copyBtn_->setToolTip(tr("Копировать ПФ (для отчёта или вставки на другую вкладку)"));
    copyBtn_->setFixedSize(24, 24);
    copyBtn_->setFocusPolicy(Qt::NoFocus);
    copyBtn_->raise();

    pasteBtn_ = new QPushButton(QStringLiteral("📌"), this);
    pasteBtn_->setObjectName(QStringLiteral("tfPasteButton"));
    pasteBtn_->setToolTip(tr("Вставить ПФ из буфера обмена"));
    pasteBtn_->setFixedSize(24, 24);
    pasteBtn_->setFocusPolicy(Qt::NoFocus);
    pasteBtn_->raise();

    connect(copyBtn_, &QPushButton::clicked, this, &TranFuncForm::copyToClipboard);
    connect(pasteBtn_, &QPushButton::clicked, this, &TranFuncForm::pasteFromClipboard);
}

void TranFuncForm::repositionClipButtons() {
    if (!delayGroup_ || !copyBtn_ || !pasteBtn_)
        return;
    // Overlay: bottom-right under delay block, zero impact on e^(−τp) layout.
    const QRect g = delayGroup_->geometry();
    constexpr int gap = 2;
    constexpr int btn = 24;
    const int x1 = g.right() - 2 * btn - gap;
    const int x2 = g.right() - btn;
    // Just under the exponent row (may sit in free space below delayGroup).
    int y = g.bottom() + 1;
    if (y + btn > height())
        y = qMax(0, height() - btn);
    if (y < g.top())
        y = g.bottom() - btn; // fall back: flush with bottom of delay
    copyBtn_->move(x1, y);
    pasteBtn_->move(x2, y);
    copyBtn_->raise();
    pasteBtn_->raise();
}

void TranFuncForm::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    repositionClipButtons();
}

void TranFuncForm::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    // Geometry is final after the first layout pass.
    QTimer::singleShot(0, this, [this] {
        for (auto* le : numerator_)
            adjustLineEditWidth(le);
        for (auto* le : denominator_)
            adjustLineEditWidth(le);
        repositionClipButtons();
    });
}

QLayout* TranFuncForm::createCoeffRow(VecLine& lineEdits, QDoubleValidator* validator, int factor) {
    auto* layout = new QHBoxLayout;
    int p = -1;
    for (auto& lineEdit : lineEdits) {
        lineEdit = new LineEdit(this);
        lineEdit->setObjectName(QStringLiteral("tfCoeff"));
        layout->addWidget(lineEdit);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        // No QDoubleValidator: it rejects empty and can restore the previous value on focus-out,
        // which prevented clearing a coefficient to zero.
        (void)validator;
        lineEdit->setStored(0.0); // display "+0" + correct initial width
        applyCoeffSign(lineEdit, 0);
        adjustLineEditWidth(lineEdit);

        auto* label = new QLabel(QString("p<sup>%1</sup>").arg(++p), this);
        label->setObjectName(QStringLiteral("tfPower"));
        applyPowerActive(label, false);
        layout->addWidget(label);

        is_active_.push_back(false);
        const std::size_t index = is_active_.size() - 1;

        // textEdited — only real keyboard/paste input (not our setText display refresh)
        connect(lineEdit, &QLineEdit::textEdited, this, [lineEdit, label](const QString& text) {
            lineEdit->dirty = true;
            if (text.trimmed().isEmpty())
                lineEdit->cleared = true;
            else
                lineEdit->cleared = false;
            if (auto corrected = correctLine(text); !corrected.isEmpty()) {
                lineEdit->blockSignals(true);
                lineEdit->setText(corrected);
                lineEdit->blockSignals(false);
            }
            const QString cur = lineEdit->text().trimmed();
            if (cur.isEmpty()) {
                applyPowerActive(label, false);
                applyCoeffSign(lineEdit, 0.0);
            } else {
                bool ok = false;
                const double value = num_format::parse(cur, &ok);
                applyPowerActive(label, ok && value != 0.0);
                applyCoeffSign(lineEdit, ok ? value : 0.0);
            }
            adjustLineEditWidth(lineEdit);
        });

        connect(lineEdit, &QLineEdit::editingFinished, this, [this, lineEdit, label, factor, p, index] {
            // Empty field always commits to 0 (even if Qt restored text before this handler).
            if (lineEdit->text().trimmed().isEmpty())
                lineEdit->cleared = true;
            lineEdit->commitIfDirty();
            lineEdit->showDisplay();
            adjustLineEditWidth(lineEdit);
            const double v = lineEdit->stored;
            applyPowerActive(label, v != 0.0);
            applyCoeffSign(lineEdit, v);

            if (v == 0.0) {
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
    }
    layout->setAlignment(Qt::AlignLeft);
    return layout;
}

void TranFuncForm::createLabelContextMenu(QLabel* label) {
    auto* aboutAction = new QAction(tr("Подробнее"), label);
    connect(aboutAction, &QAction::triggered, this, [this] {
        if (!tf_)
            return;
        TranFuncDialog dialog(*tf_, this);
        dialog.exec();
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

QString TranFuncForm::formatCoeff(double value) {
    return num_format::formatCoeff(value, num_format::SIGNIFICANT_DIGITS, /*plus_sign=*/true);
}

void TranFuncForm::setFieldValue(LineEdit* lineEdit, double value) {
    lineEdit->setStored(value);
    applyCoeffSign(lineEdit, lineEdit->stored);
    adjustLineEditWidth(lineEdit);
}

void TranFuncForm::setCoeffFields(VecLine& fields, const Vec& highToLow) {
    for (auto* le : fields)
        setFieldValue(le, 0.0);
    if (highToLow.empty())
        return;
    const int n = static_cast<int>(fields.size());
    const int m = static_cast<int>(highToLow.size());
    // highToLow[0] = highest power; fields[j] = p^j
    for (int j = 0; j < n; ++j) {
        const int idx = m - 1 - j;
        if (idx >= 0)
            setFieldValue(fields[static_cast<std::size_t>(j)], highToLow[static_cast<std::size_t>(idx)]);
    }
}

void TranFuncForm::rebuildActivityMask() {
    id_ = 0;
    is_active_.assign(numerator_.size() + denominator_.size(), false);
    const int n = static_cast<int>(numerator_.size());
    for (int p = 0; p < n; ++p) {
        if (numerator_[static_cast<std::size_t>(p)]->stored != 0.0) {
            is_active_[static_cast<std::size_t>(p)] = true;
            id_ += 1 << p;
        }
    }
    for (int p = 0; p < static_cast<int>(denominator_.size()); ++p) {
        const std::size_t index = static_cast<std::size_t>(n + p);
        if (denominator_[static_cast<std::size_t>(p)]->stored != 0.0) {
            is_active_[index] = true;
            id_ += (1 << n) << p;
        }
    }
}

QString TranFuncForm::polyToString(const Vec& highToLow) {
    if (highToLow.empty())
        return QStringLiteral("0");
    QStringList terms;
    const int deg = static_cast<int>(highToLow.size()) - 1;
    for (int k = 0; k <= deg; ++k) {
        const double c = highToLow[static_cast<std::size_t>(k)];
        if (c == 0.0)
            continue;
        const int power = deg - k;
        QString coef = QString::number(c, 'g', 12);
        coef.replace(',', '.');
        QString term;
        if (power == 0)
            term = coef;
        else if (power == 1) {
            if (c == 1.0)
                term = QStringLiteral("p");
            else if (c == -1.0)
                term = QStringLiteral("-p");
            else
                term = coef + QStringLiteral("·p");
        } else {
            if (c == 1.0)
                term = QStringLiteral("p^%1").arg(power);
            else if (c == -1.0)
                term = QStringLiteral("-p^%1").arg(power);
            else
                term = coef + QStringLiteral("·p^%1").arg(power);
        }
        terms.push_back(term);
    }
    if (terms.isEmpty())
        return QStringLiteral("0");
    QString s = terms.front();
    for (int i = 1; i < terms.size(); ++i) {
        if (terms[i].startsWith(QLatin1Char('-')))
            s += QStringLiteral(" ") + terms[i];
        else
            s += QStringLiteral(" + ") + terms[i];
    }
    return s;
}

QString TranFuncForm::exportText() const {
    const auto num = numerator();
    const auto den = denominator();
    const double tau = delayTime();

    QStringList numParts;
    QStringList denParts;
    // Export stored precision (16 sig digits), not display-rounded values.
    for (double v : num)
        numParts << num_format::format(v, num_format::STORED_DIGITS);
    for (double v : den)
        denParts << num_format::format(v, num_format::STORED_DIGITS);

    QString human = QStringLiteral("W(p) = (%1) / (%2)")
                        .arg(num_format::polyPlainLowFirst(num), num_format::polyPlainLowFirst(den));
    if (tau != 0.0)
        human += QStringLiteral(" · e^(-%1 p)").arg(num_format::formatFull(tau));

    return QStringLiteral(
               "RegValve-TF-v1\n"
               "num: %1\n"
               "den: %2\n"
               "tau: %3\n"
               "\n"
               "%4\n")
        .arg(numParts.join(QLatin1Char(' ')), denParts.join(QLatin1Char(' ')), num_format::formatFull(tau), human);
}

bool TranFuncForm::importText(const QString& text) {
    const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts);
    Vec num, den;
    double tau = 0.0;
    bool hasHeader = false;
    bool gotNum = false;
    bool gotDen = false;

    for (QString line : lines) {
        line = line.trimmed();
        if (line.startsWith(QStringLiteral("RegValve-TF-v1"), Qt::CaseInsensitive)) {
            hasHeader = true;
            continue;
        }
        if (line.startsWith(QStringLiteral("num:"), Qt::CaseInsensitive)) {
            num = parseCoeffList(line.mid(4));
            gotNum = !num.empty();
            continue;
        }
        if (line.startsWith(QStringLiteral("den:"), Qt::CaseInsensitive)) {
            den = parseCoeffList(line.mid(4));
            gotDen = !den.empty();
            continue;
        }
        if (line.startsWith(QStringLiteral("tau:"), Qt::CaseInsensitive)) {
            bool ok = false;
            tau = line.mid(4).trimmed().toDouble(&ok);
            if (!ok)
                tau = 0.0;
            continue;
        }
    }

    if (!hasHeader || !gotNum || !gotDen)
        return false;

    setCoeffFields(numerator_, num);
    setCoeffFields(denominator_, den);
    delayElement_->setValue(std::clamp(tau, 0.0, 30.0));
    rebuildActivityMask();
    updateNameLabel();
    emit coefficientsChanged();
    return true;
}

void TranFuncForm::copyToClipboard() {
    QApplication::clipboard()->setText(exportText());
    if (copyBtn_)
        QToolTip::showText(copyBtn_->mapToGlobal(QPoint(0, copyBtn_->height())), tr("ПФ скопирована"), copyBtn_,
                           QRect(), 1500);
}

void TranFuncForm::pasteFromClipboard() {
    const QString text = QApplication::clipboard()->text();
    if (!importText(text)) {
        QMessageBox::information(this, tr("Вставка ПФ"),
                                 tr("В буфере нет данных формата RegValve-TF-v1.\n"
                                    "Скопируйте ПФ кнопкой 📋 на вкладке «Анализ» или «Синтез»."));
        return;
    }
    if (pasteBtn_)
        QToolTip::showText(pasteBtn_->mapToGlobal(QPoint(0, pasteBtn_->height())), tr("ПФ вставлена"), pasteBtn_,
                           QRect(), 1500);
}

double TranFuncForm::getValue(QString text) {
    return num_format::parse(std::move(text));
}

void TranFuncForm::adjustLineEditWidth(QLineEdit* lineEdit) {
    const int width = qMax(36, lineEdit->fontMetrics().horizontalAdvance(lineEdit->text()) + 16);
    lineEdit->setFixedWidth(width);
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
    // Use stored 16-digit values, not the 6-digit display text.
    for (std::size_t i = 0; i < size; ++i)
        values[i] = lineEdits[i]->stored;
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
