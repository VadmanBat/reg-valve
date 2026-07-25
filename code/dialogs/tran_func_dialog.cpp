#include "code/dialogs/tran_func_dialog.h"
#include "ui_tran_func_dialog.h"

#include <QHeaderView>
#include <QTableWidgetItem>

#include <cmath>
#include <numbers>
#include <sstream>

TranFuncDialog::TranFuncDialog(const numina::TransferFunction& tf, QWidget* parent)
    : QDialog(parent), ui(new Ui::TranFuncDialog), tf_(tf) {
    ui->setupUi(this);
    ui->polesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    fillPoles();

    const auto& h = tf_.transientSolution().htmlString();
    const auto& w = tf_.impulseSolution().htmlString();
    if (!h.empty())
        ui->htTextEdit->setHtml(QString::fromStdString(h));
    if (!w.empty())
        ui->wtTextEdit->setHtml(QString::fromStdString(w));

    connect(ui->okButton, &QPushButton::clicked, this, &TranFuncDialog::close);
}

TranFuncDialog::~TranFuncDialog() {
    delete ui;
}

QColor TranFuncDialog::rootColor(double value) {
    if (value < 0)
        return Qt::black;
    if (value <= 0.5)
        return {static_cast<int>(255 * (value * 2)), static_cast<int>(255 * (1 - value * 2)), 0};
    return {255, static_cast<int>(255 * (1 - (value - 0.5) * 2)), 0};
}

void TranFuncDialog::fillPoles() {
    ui->polesTable->setRowCount(0);
    const auto& poles = tf_.getPoles();
    const auto& reals = poles.first;
    const auto& comps = poles.second;

    auto addRow = [this](std::complex<double> pole, double refRe) {
        const int row = ui->polesTable->rowCount();
        ui->polesTable->insertRow(row);
        auto* reItem = new QTableWidgetItem;
        if (pole.real() < 0) {
            reItem->setText(QString::number(pole.real()));
            reItem->setForeground(QBrush(Qt::blue));
        } else {
            reItem->setText(QStringLiteral("+") + QString::number(pole.real()));
            reItem->setForeground(QBrush(Qt::red));
        }
        ui->polesTable->setItem(row, 0, reItem);

        if (std::abs(pole.imag()) > 1e-10) {
            ui->polesTable->setItem(row, 1,
                                    new QTableWidgetItem(QStringLiteral("±") + QString::number(std::abs(pole.imag()))));
            ui->polesTable->setItem(
                row, 5,
                new QTableWidgetItem(QString::number(pole.real() != 0 ? -pole.real() / std::abs(pole) : 0.0, 'f', 4)));
            ui->polesTable->setItem(
                row, 6, new QTableWidgetItem(QString::number(2 * std::numbers::pi / std::abs(pole.imag()), 'f', 4)));
        }

        ui->polesTable->setItem(row, 2, new QTableWidgetItem(QString::number(std::abs(pole), 'f', 4)));
        ui->polesTable->setItem(
            row, 3, new QTableWidgetItem(QString::number(std::arg(pole) * 180.0 / std::numbers::pi, 'f', 4)));
        ui->polesTable->setItem(
            row, 4,
            new QTableWidgetItem(QString::number(
                pole.real() != 0 ? -1.0 / pole.real() : std::numeric_limits<double>::infinity(), 'f', 4)));

        auto* colorItem = new QTableWidgetItem;
        if (refRe != 0)
            colorItem->setBackground(rootColor(refRe / pole.real()));
        ui->polesTable->setItem(row, 7, colorItem);
    };

    double refRe = 0;
    if (!reals.empty())
        refRe = reals.front().first;
    else if (!comps.empty())
        refRe = comps.front().first.real();

    for (const auto& [r, mult] : reals) {
        for (std::size_t k = 0; k < mult; ++k)
            addRow({r, 0.0}, refRe);
    }
    for (const auto& [c, mult] : comps) {
        for (std::size_t k = 0; k < mult; ++k)
            addRow(c, refRe);
    }
}
