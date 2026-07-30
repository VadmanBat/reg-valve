#include "code/widgets/tran-func-form.h"

#include "code/dialogs/tran-func-dialog.h"
#include "code/util/format.hxx"
#include "code/util/style-core.hpp"

#include <QAbstractSpinBox>
#include <QApplication>
#include <QClipboard>
#include <QDoubleSpinBox>
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
#include <QStyleOptionSpinBox>
#include <QTimer>
#include <QToolTip>

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
        const QString full = num_format::formatCoeff(stored, num_format::STORED_DIGITS, true);
        blockSignals(true);
        setText(full);
        blockSignals(false);
        dirty   = false;
        cleared = false;
    }

    void commitIfDirty() {
        if (!dirty)
            return;
        const QString t = text().trimmed();
        if (cleared || t.isEmpty() || t == QLatin1String("+") || t == QLatin1String("-")) {
            stored = 0.0;
        } else {
            bool ok = false;
            const double v = num_format::parse(t, &ok);
            stored = ok ? num_format::roundSignificant(v, num_format::STORED_DIGITS) : 0.0;
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
};

namespace {

void apply_coeff_sign(QLineEdit* line_edit, double value) {
    const char* sign = "zero";
    if (value > 0)
        sign = "positive";
    else if (value < 0)
        sign = "negative";
    style_util::setProperty(line_edit, "coeffSign", QByteArray(sign));
}

void apply_power_active(QLabel* label, bool active) {
    style_util::setProperty(label, "powerActive", active ? QByteArray("true") : QByteArray("false"));
}

std::vector<double> parse_coeff_list(QString line) {
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

    auto* title_label = new QLabel(title, this);
    title_label->setObjectName(QStringLiteral("tfTitle"));
    title_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    create_label_context_menu(title_label);

    auto* divider = new QFrame(this);
    divider->setObjectName(QStringLiteral("tfDivider"));
    divider->setFrameShape(QFrame::NoFrame);
    divider->setFixedHeight(2);
    divider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    divider->setMinimumWidth(120);
    divider->setAttribute(Qt::WA_StyledBackground, true);

    auto* frac_grid = new QGridLayout;
    frac_grid->setHorizontalSpacing(8);
    frac_grid->setVerticalSpacing(6);
    frac_grid->setContentsMargins(0, 0, 0, 0);
    frac_grid->addWidget(title_label, 0, 0, 3, 1, Qt::AlignRight | Qt::AlignVCenter);
    frac_grid->addLayout(create_coeff_row(numerator_, 1), 0, 1);
    frac_grid->addWidget(divider, 1, 1);
    frac_grid->addLayout(create_coeff_row(denominator_, 1 << static_cast<int>(n)), 2, 1);
    frac_grid->setColumnStretch(1, 1);
    frac_grid->setRowMinimumHeight(1, 2);

    auto* root = new QHBoxLayout(this);
    root->setSpacing(8);
    root->setAlignment(Qt::AlignVCenter);
    root->addLayout(frac_grid, 1);

    auto* mul_dot = new QLabel(QString::fromUtf8("·"), this);
    mul_dot->setObjectName(QStringLiteral("tfMulDot"));
    mul_dot->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    root->addWidget(mul_dot, 0, Qt::AlignVCenter);

    delay_group_ = new QWidget(this);
    delay_group_->setObjectName(QStringLiteral("tfDelayGroup"));
    delay_group_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    auto* delay_root = new QHBoxLayout(delay_group_);
    delay_root->setContentsMargins(0, 0, 0, 0);
    delay_root->setSpacing(2);

    auto* e_base = new QLabel(QStringLiteral("e"), delay_group_);
    e_base->setObjectName(QStringLiteral("tfDelayBase"));
    e_base->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    auto* exp_group = new QWidget(delay_group_);
    exp_group->setObjectName(QStringLiteral("tfDelayExp"));
    auto* exp_layout = new QHBoxLayout(exp_group);
    exp_layout->setContentsMargins(0, 0, 0, 8);
    exp_layout->setSpacing(2);

    auto* minus_lab = new QLabel(QString::fromUtf8("−"), exp_group);
    minus_lab->setObjectName(QStringLiteral("tfDelayMinus"));
    minus_lab->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    delay_element_ = new QDoubleSpinBox(exp_group);
    delay_element_->setObjectName(QStringLiteral("tfDelay"));
    delay_element_->setDecimals(2);
    delay_element_->setSingleStep(0.01);
    delay_element_->setRange(0.0, 30.0);
    delay_element_->setValue(0.0);
    delay_element_->setKeyboardTracking(false);
    delay_element_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    delay_element_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    delay_element_->setToolTip(tr("Постоянная запаздывания τ, с (0…30). Ввод с клавиатуры."));
    delay_element_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    {
        QStyleOptionSpinBox opt;
        opt.initFrom(delay_element_);
        opt.frame         = true;
        opt.buttonSymbols = QAbstractSpinBox::NoButtons;
        opt.stepEnabled   = QAbstractSpinBox::StepNone;
        const QFontMetrics fm(delay_element_->font());
        const int extra     = 2 * fm.horizontalAdvance(QLatin1Char('0'));
        const int text_w    = fm.horizontalAdvance(QStringLiteral("30.00")) + extra;
        const QSize text_sz(text_w + 4, fm.height());
        const QSize sz =
            delay_element_->style()->sizeFromContents(QStyle::CT_SpinBox, &opt, text_sz, delay_element_);
        delay_element_->setFixedSize(sz.expandedTo(text_sz + QSize(14, 6)));
    }

    auto* p_lab = new QLabel(QStringLiteral("p"), exp_group);
    p_lab->setObjectName(QStringLiteral("tfDelayP"));
    p_lab->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    exp_layout->addWidget(minus_lab);
    exp_layout->addWidget(delay_element_);
    exp_layout->addWidget(p_lab);

    delay_root->addWidget(e_base, 0, Qt::AlignBottom);
    delay_root->addWidget(exp_group, 0, Qt::AlignTop);

    root->addWidget(delay_group_, 0, Qt::AlignVCenter);

    copy_btn_ = new QPushButton(QStringLiteral("📋"), this);
    copy_btn_->setObjectName(QStringLiteral("tfCopyButton"));
    copy_btn_->setToolTip(tr("Копировать ПФ (для отчёта или вставки на другую вкладку)"));
    copy_btn_->setFixedSize(24, 24);
    copy_btn_->setFocusPolicy(Qt::NoFocus);
    copy_btn_->raise();

    paste_btn_ = new QPushButton(QStringLiteral("📌"), this);
    paste_btn_->setObjectName(QStringLiteral("tfPasteButton"));
    paste_btn_->setToolTip(tr("Вставить ПФ из буфера обмена"));
    paste_btn_->setFixedSize(24, 24);
    paste_btn_->setFocusPolicy(Qt::NoFocus);
    paste_btn_->raise();

    connect(copy_btn_, &QPushButton::clicked, this, &TranFuncForm::copyToClipboard);
    connect(paste_btn_, &QPushButton::clicked, this, &TranFuncForm::pasteFromClipboard);
}

void TranFuncForm::reposition_clip_buttons() {
    const QRect g = delay_group_->geometry();
    constexpr int gap = 2;
    constexpr int btn = 24;
    const int x1 = g.right() - 2 * btn - gap;
    const int x2 = g.right() - btn;
    int y = g.bottom() + 1;
    if (y + btn > height())
        y = qMax(0, height() - btn);
    if (y < g.top())
        y = g.bottom() - btn;
    copy_btn_->move(x1, y);
    paste_btn_->move(x2, y);
    copy_btn_->raise();
    paste_btn_->raise();
}

void TranFuncForm::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    reposition_clip_buttons();
}

void TranFuncForm::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    QTimer::singleShot(0, this, [this] {
        for (auto* le : numerator_)
            adjust_line_edit_width(le);
        for (auto* le : denominator_)
            adjust_line_edit_width(le);
        reposition_clip_buttons();
    });
}

QLayout* TranFuncForm::create_coeff_row(VecLine& line_edits, int factor) {
    auto* layout = new QHBoxLayout;
    int p = -1;
    for (auto& line_edit : line_edits) {
        line_edit = new LineEdit(this);
        line_edit->setObjectName(QStringLiteral("tfCoeff"));
        layout->addWidget(line_edit);
        line_edit->setAlignment(Qt::AlignRight);
        line_edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        line_edit->setStored(0.0);
        apply_coeff_sign(line_edit, 0);
        adjust_line_edit_width(line_edit);

        auto* label = new QLabel(QString("p<sup>%1</sup>").arg(++p), this);
        label->setObjectName(QStringLiteral("tfPower"));
        apply_power_active(label, false);
        layout->addWidget(label);

        is_active_.push_back(false);
        const std::size_t index = is_active_.size() - 1;

        connect(line_edit, &QLineEdit::textEdited, this, [line_edit, label](const QString& text) {
            line_edit->dirty = true;
            line_edit->cleared = text.trimmed().isEmpty();
            if (auto corrected = correct_line(text); !corrected.isEmpty()) {
                line_edit->blockSignals(true);
                line_edit->setText(corrected);
                line_edit->blockSignals(false);
            }
            const QString cur = line_edit->text().trimmed();
            if (cur.isEmpty()) {
                apply_power_active(label, false);
                apply_coeff_sign(line_edit, 0.0);
            } else {
                bool ok = false;
                const double value = num_format::parse(cur, &ok);
                apply_power_active(label, ok && value != 0.0);
                apply_coeff_sign(line_edit, ok ? value : 0.0);
            }
            adjust_line_edit_width(line_edit);
        });

        connect(line_edit, &QLineEdit::editingFinished, this, [this, line_edit, label, factor, p, index] {
            if (line_edit->text().trimmed().isEmpty())
                line_edit->cleared = true;
            line_edit->commitIfDirty();
            line_edit->showDisplay();
            adjust_line_edit_width(line_edit);
            const double v = line_edit->stored;
            apply_power_active(label, v != 0.0);
            apply_coeff_sign(line_edit, v);

            if (v == 0.0) {
                if (is_active_[index]) {
                    id_ -= factor << p;
                    is_active_[index] = false;
                }
            } else if (!is_active_[index]) {
                id_ += factor << p;
                is_active_[index] = true;
            }
        });
    }
    layout->setAlignment(Qt::AlignLeft);
    return layout;
}

void TranFuncForm::create_label_context_menu(QLabel* label) {
    auto* about_action = new QAction(tr("Подробнее"), label);
    connect(about_action, &QAction::triggered, this, [this] {
        if (!tf_)
            return;
        TranFuncDialog dialog(*tf_, this);
        dialog.exec();
    });
    auto* menu = new QMenu(label);
    menu->addAction(about_action);
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(label, &QLabel::customContextMenuRequested, this, [menu, label](const QPoint& pos) {
        menu->exec(label->mapToGlobal(pos));
    });
}

void TranFuncForm::update_name_label() {
    if (name_label_)
        name_label_->setText(linkName());
}

void TranFuncForm::bindNameLabel(QLabel* label) {
    name_label_ = label;
    for (auto* le : numerator_)
        connect(le, &QLineEdit::editingFinished, this, &TranFuncForm::update_name_label);
    for (auto* le : denominator_)
        connect(le, &QLineEdit::editingFinished, this, &TranFuncForm::update_name_label);
}

void TranFuncForm::setTransferFunction(const numina::TransferFunction* tf) {
    tf_ = tf;
}

void TranFuncForm::set_field_value(LineEdit* line_edit, double value) {
    line_edit->setStored(value);
    apply_coeff_sign(line_edit, line_edit->stored);
    adjust_line_edit_width(line_edit);
}

void TranFuncForm::set_coeff_fields(VecLine& fields, const Vec& high_to_low) {
    for (auto* le : fields)
        set_field_value(le, 0.0);
    if (high_to_low.empty())
        return;
    const int n = static_cast<int>(fields.size());
    const int m = static_cast<int>(high_to_low.size());
    for (int j = 0; j < n; ++j) {
        const int idx = m - 1 - j;
        if (idx >= 0)
            set_field_value(fields[static_cast<std::size_t>(j)], high_to_low[static_cast<std::size_t>(idx)]);
    }
}

void TranFuncForm::rebuild_activity_mask() {
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

QString TranFuncForm::exportText() const {
    const auto num = numerator();
    const auto den = denominator();
    const double tau = delayTime();

    QStringList num_parts;
    QStringList den_parts;
    for (double v : num)
        num_parts << num_format::format(v, num_format::STORED_DIGITS);
    for (double v : den)
        den_parts << num_format::format(v, num_format::STORED_DIGITS);

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
        .arg(num_parts.join(QLatin1Char(' ')), den_parts.join(QLatin1Char(' ')), num_format::formatFull(tau),
             human);
}

bool TranFuncForm::importText(const QString& text) {
    const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts);
    Vec num, den;
    double tau = 0.0;
    bool has_header = false;
    bool got_num = false;
    bool got_den = false;

    for (QString line : lines) {
        line = line.trimmed();
        if (line.startsWith(QStringLiteral("RegValve-TF-v1"), Qt::CaseInsensitive)) {
            has_header = true;
            continue;
        }
        if (line.startsWith(QStringLiteral("num:"), Qt::CaseInsensitive)) {
            num = parse_coeff_list(line.mid(4));
            got_num = !num.empty();
            continue;
        }
        if (line.startsWith(QStringLiteral("den:"), Qt::CaseInsensitive)) {
            den = parse_coeff_list(line.mid(4));
            got_den = !den.empty();
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

    if (!has_header || !got_num || !got_den)
        return false;

    set_coeff_fields(numerator_, num);
    set_coeff_fields(denominator_, den);
    delay_element_->setValue(std::clamp(tau, 0.0, 30.0));
    rebuild_activity_mask();
    update_name_label();
    return true;
}

void TranFuncForm::copyToClipboard() {
    QApplication::clipboard()->setText(exportText());
    QToolTip::showText(copy_btn_->mapToGlobal(QPoint(0, copy_btn_->height())), tr("ПФ скопирована"), copy_btn_,
                       QRect(), 1500);
}

void TranFuncForm::pasteFromClipboard() {
    if (!importText(QApplication::clipboard()->text())) {
        QMessageBox::information(this, tr("Вставка ПФ"),
                                 tr("В буфере нет данных формата RegValve-TF-v1.\n"
                                    "Скопируйте ПФ кнопкой 📋 на вкладке «Анализ» или «Синтез»."));
        return;
    }
    QToolTip::showText(paste_btn_->mapToGlobal(QPoint(0, paste_btn_->height())), tr("ПФ вставлена"), paste_btn_,
                       QRect(), 1500);
}

void TranFuncForm::adjust_line_edit_width(QLineEdit* line_edit) {
    const int width = qMax(36, line_edit->fontMetrics().horizontalAdvance(line_edit->text()) + 16);
    line_edit->setFixedWidth(width);
}

TranFuncForm::Vec TranFuncForm::reverse_optimize(const Vec& container) {
    auto first = container.rbegin();
    const auto last = container.rend();
    if (first == last)
        return {};
    while (first != last && *first == 0)
        ++first;
    return {first, last};
}

TranFuncForm::Vec TranFuncForm::get_line_edit_data(const VecLine& line_edits) {
    const auto size = line_edits.size();
    Vec values(size);
    for (std::size_t i = 0; i < size; ++i)
        values[i] = line_edits[i]->stored;
    return reverse_optimize(values);
}

QString TranFuncForm::correct_line(const QString& text) {
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
    return get_line_edit_data(numerator_);
}

std::vector<double> TranFuncForm::denominator() const {
    return get_line_edit_data(denominator_);
}

bool TranFuncForm::hasDelay() const {
    return delay_element_->value() != 0;
}

double TranFuncForm::delayTime() const {
    return delay_element_->value();
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
    case 1 + 7 * 64: {
        // ζ = a1 / (2 √a2); use stored values (not display text).
        const double a1 = denominator_[1]->stored;
        const double a2 = denominator_[2]->stored;
        if (a2 > 0.0 && a1 / (2.0 * std::sqrt(a2)) < 1.0)
            return tr("Колебательное");
        return tr("Инерционное 2-го порядка (апериодическое)");
    }
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
