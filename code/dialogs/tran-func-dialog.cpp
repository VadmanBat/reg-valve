#include "code/dialogs/tran-func-dialog.h"
#include "ui_tran-func-dialog.h"

#include "code/util/format.hxx"

#include <QAbstractItemView>
#include <QBrush>
#include <QHeaderView>
#include <QTableWidgetItem>

#include <cmath>
#include <complex>
#include <numbers>

TranFuncDialog::TranFuncDialog(const numina::TransferFunction& tf, QWidget* parent)
    : QDialog(parent), ui(new Ui::TranFuncDialog), tf_(tf) {
    ui->setupUi(this);

    ui->polesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->polesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    fillPoles();

    ui->htTextEdit->setHtml(QString::fromStdString(tf_.transientSolution().htmlString()));
    ui->wtTextEdit->setHtml(QString::fromStdString(tf_.impulseSolution().htmlString()));

    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
}

TranFuncDialog::~TranFuncDialog() {
    delete ui;
}

QColor TranFuncDialog::rootColor(double value) {
    if (value < 0)
        return QColor(80, 80, 80);
    if (value <= 0.5)
        return {static_cast<int>(255 * (value * 2)), static_cast<int>(255 * (1 - value * 2)), 0};
    return {255, static_cast<int>(255 * (1 - (value - 0.5) * 2)), 0};
}

void TranFuncDialog::fillPoles() {
    ui->polesTable->setRowCount(0);

    const auto& reals = tf_.getPoles().first;
    const auto& comps = tf_.getPoles().second;

    bool any_multi = false;
    for (const auto& e : reals)
        if (e.second > 1) {
            any_multi = true;
            break;
        }
    if (!any_multi)
        for (const auto& e : comps)
            if (e.second > 1) {
                any_multi = true;
                break;
            }

    constexpr int kCol = 8;
    ui->polesTable->setColumnCount(any_multi ? 9 : 8);
    if (any_multi)
        ui->polesTable->setHorizontalHeaderItem(kCol, new QTableWidgetItem(tr("k")));

    double refRe = 0.0;
    if (!reals.empty())
        refRe = reals.front().first;
    else if (!comps.empty())
        refRe = comps.front().first.real();

    auto addRow = [&](std::complex<double> pole, std::size_t mult) {
        const int row = ui->polesTable->rowCount();
        ui->polesTable->insertRow(row);

        auto* reItem = new QTableWidgetItem(
            pole.real() < 0 ? num_format::format(pole.real())
                            : QStringLiteral("+") + num_format::format(pole.real()));
        reItem->setForeground(QBrush(pole.real() < 0 ? QColor(0x80, 0xd8, 0xff) : QColor(0xff, 0xb0, 0xc8)));
        ui->polesTable->setItem(row, 0, reItem);

        const double abs_p = std::abs(pole);
        ui->polesTable->setItem(row, 2, new QTableWidgetItem(num_format::format(abs_p)));
        ui->polesTable->setItem(row, 3,
                                new QTableWidgetItem(num_format::format(std::arg(pole) * 180.0 / std::numbers::pi)));
        ui->polesTable->setItem(
            row, 4,
            new QTableWidgetItem(pole.real() != 0.0 ? num_format::format(-1.0 / pole.real())
                                                    : QStringLiteral("∞")));

        if (std::abs(pole.imag()) > 1e-10) {
            ui->polesTable->setItem(
                row, 1, new QTableWidgetItem(QStringLiteral("±") + num_format::format(std::abs(pole.imag()))));
            ui->polesTable->setItem(row, 5, new QTableWidgetItem(num_format::format(-pole.real() / abs_p)));
            ui->polesTable->setItem(
                row, 6, new QTableWidgetItem(num_format::format(2.0 * std::numbers::pi / std::abs(pole.imag()))));
        }

        auto* colorItem = new QTableWidgetItem;
        if (refRe != 0.0 && pole.real() != 0.0)
            colorItem->setBackground(rootColor(refRe / pole.real()));
        ui->polesTable->setItem(row, 7, colorItem);

        if (any_multi)
            ui->polesTable->setItem(row, kCol, new QTableWidgetItem(QString::number(static_cast<int>(mult))));
    };

    for (const auto& [r, mult] : reals)
        addRow({r, 0.0}, mult);
    for (const auto& [c, mult] : comps)
        addRow(c, mult);
}
