#include "code/widgets/tf_display_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QToolTip>
#include <QVBoxLayout>

TfDisplayWidget::TfDisplayWidget(const QString& title, QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("TfDisplayWidget"));

    titleLabel_ = new QLabel(title, this);
    titleLabel_->setObjectName(QStringLiteral("tfTitle"));
    titleLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    numLabel_ = new QLabel(QStringLiteral("—"), this);
    numLabel_->setObjectName(QStringLiteral("tfPolyText"));
    numLabel_->setAlignment(Qt::AlignCenter);
    numLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);

    denLabel_ = new QLabel(QStringLiteral("—"), this);
    denLabel_->setObjectName(QStringLiteral("tfPolyText"));
    denLabel_->setAlignment(Qt::AlignCenter);
    denLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto* divider = new QFrame(this);
    divider->setObjectName(QStringLiteral("tfDivider"));
    divider->setFrameShape(QFrame::NoFrame);
    divider->setFixedHeight(2);
    divider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    divider->setMinimumWidth(120);
    divider->setAttribute(Qt::WA_StyledBackground, true);

    delayLabel_ = new QLabel(this);
    delayLabel_->setObjectName(QStringLiteral("tfDelayText"));
    delayLabel_->setVisible(false);

    copyBtn_ = new QPushButton(QStringLiteral("⎘"), this);
    copyBtn_->setObjectName(QStringLiteral("tfCopyButton"));
    copyBtn_->setToolTip(tr("Копировать ПФ"));
    copyBtn_->setFixedSize(28, 28);
    copyBtn_->setEnabled(false);
    connect(copyBtn_, &QPushButton::clicked, this, &TfDisplayWidget::copyToClipboard);

    auto* frac = new QGridLayout;
    frac->setContentsMargins(0, 0, 0, 0);
    frac->setHorizontalSpacing(8);
    frac->setVerticalSpacing(4);
    frac->addWidget(titleLabel_, 0, 0, 3, 1, Qt::AlignRight | Qt::AlignVCenter);
    frac->addWidget(numLabel_, 0, 1);
    frac->addWidget(divider, 1, 1);
    frac->addWidget(denLabel_, 2, 1);
    frac->addWidget(delayLabel_, 0, 2, 3, 1, Qt::AlignLeft | Qt::AlignVCenter);

    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 0, 0, 0);
    row->addLayout(frac, 1);
    row->addWidget(copyBtn_, 0, Qt::AlignTop);
    row->addStretch(1);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->addLayout(row);
}

QString TfDisplayWidget::formatCoeff(double value) {
    QString s = QString::number(value, 'g', 12);
    s.replace(',', '.');
    return s;
}

QString TfDisplayWidget::polyToString(const Vec& highToLow) {
    if (highToLow.empty())
        return QStringLiteral("0");
    QStringList terms;
    const int deg = static_cast<int>(highToLow.size()) - 1;
    for (int k = 0; k <= deg; ++k) {
        const double c = highToLow[static_cast<std::size_t>(k)];
        if (c == 0.0)
            continue;
        const int power = deg - k;
        const QString coef = formatCoeff(c);
        QString term;
        if (power == 0)
            term = coef;
        else if (power == 1) {
            if (c == 1.0)
                term = QStringLiteral("p");
            else if (c == -1.0)
                term = QStringLiteral("−p");
            else
                term = coef + QStringLiteral("·p");
        } else {
            if (c == 1.0)
                term = QStringLiteral("p^%1").arg(power);
            else if (c == -1.0)
                term = QStringLiteral("−p^%1").arg(power);
            else
                term = coef + QStringLiteral("·p^%1").arg(power);
        }
        terms.push_back(term);
    }
    if (terms.isEmpty())
        return QStringLiteral("0");
    QString out = terms.front();
    for (int i = 1; i < terms.size(); ++i) {
        if (terms[i].startsWith(QLatin1Char('-')) || terms[i].startsWith(QStringLiteral("−")))
            out += QStringLiteral(" ") + terms[i];
        else
            out += QStringLiteral(" + ") + terms[i];
    }
    return out;
}

void TfDisplayWidget::setPolys(const Vec& num, const Vec& den, double tau) {
    num_   = num;
    den_   = den;
    tau_   = tau;
    empty_ = num.empty() && den.empty();

    numLabel_->setText(empty_ ? QStringLiteral("—") : polyToString(num_));
    denLabel_->setText(empty_ ? QStringLiteral("—") : polyToString(den_));

    if (!empty_ && tau_ != 0.0) {
        delayLabel_->setText(QStringLiteral("· e^(−%1 p)").arg(QString::number(tau_, 'f', 2)));
        delayLabel_->setVisible(true);
    } else {
        delayLabel_->clear();
        delayLabel_->setVisible(false);
    }
    copyBtn_->setEnabled(!empty_);
}

void TfDisplayWidget::setTransferFunction(const numina::TransferFunction& tf) {
    setPolys(tf.getNumerator().vector(), tf.getDenominator().vector(), 0.0);
}

void TfDisplayWidget::clear() {
    setPolys({}, {}, 0.0);
}

QString TfDisplayWidget::humanText() const {
    if (empty_)
        return {};
    QString human = QStringLiteral("W(p) = (%1) / (%2)").arg(polyToString(num_), polyToString(den_));
    if (tau_ != 0.0)
        human += QStringLiteral(" · e^(−%1 p)").arg(QString::number(tau_, 'f', 2));
    return human;
}

QString TfDisplayWidget::exportText() const {
    if (empty_)
        return {};
    QStringList numParts, denParts;
    for (double v : num_)
        numParts << formatCoeff(v);
    for (double v : den_)
        denParts << formatCoeff(v);
    return QStringLiteral(
               "RegValve-TF-v1\n"
               "num: %1\n"
               "den: %2\n"
               "tau: %3\n"
               "\n"
               "%4\n")
        .arg(numParts.join(QLatin1Char(' ')), denParts.join(QLatin1Char(' ')), QString::number(tau_, 'f', 2),
             humanText());
}

void TfDisplayWidget::copyToClipboard() {
    if (empty_)
        return;
    QApplication::clipboard()->setText(exportText());
    if (copyBtn_)
        QToolTip::showText(copyBtn_->mapToGlobal(QPoint(0, copyBtn_->height())), tr("ПФ скопирована"), copyBtn_,
                           QRect(), 1500);
}
