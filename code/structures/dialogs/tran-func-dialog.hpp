#ifndef REGVALVE_TRAN_FUNC_DIALOG_HPP
#define REGVALVE_TRAN_FUNC_DIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVector>
#include <complex>
#include <QTableWidget>
#include <QHeaderView>

#include "code/transfer-function/transfer-function.hpp"

class TranFuncDialog : public QDialog {
Q_OBJECT

public:
    explicit TranFuncDialog(const TransferFunction& tf, QWidget* parent = nullptr) :
            QDialog(parent), tf(tf)
    {
        setWindowTitle("Информация о передаточной функции");
        setWindowIcon(QIcon::fromTheme("dialog-information"));
        setGeometry(300, 300, 800, 600);
        initUI();
    }

private:
    const TransferFunction& tf;

    void initUI() {
        auto mainLayout = new QVBoxLayout(this);

        auto titleLabel = new QLabel("Корневая характеристика системы по передаточной функции", this);
        auto polesLabel = new QLabel("<b>Полюса (корни) системы:</b>", this);
        auto h_t_Label  = new QLabel("<b>Переходная функция h(t):</b>", this);
        auto w_t_Label  = new QLabel("<b>Импульсная функция w(t):</b>", this);

        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        titleLabel->setAlignment(Qt::AlignCenter);

        auto polesTable = new QTableWidget(this);
        formatPoles(polesTable);

        auto h_t_TextEdit = new QTextEdit(this);
        auto w_t_TextEdit = new QTextEdit(this);

        h_t_TextEdit->setReadOnly(true);
        h_t_TextEdit->setStyleSheet("font-size: 14px;");
        if (auto str = tf.transientFunction.string(); !str.empty())
            h_t_TextEdit->setHtml(str.c_str());

        w_t_TextEdit->setReadOnly(true);
        w_t_TextEdit->setStyleSheet("font-size: 14px;");
        if (auto str = tf.impulseFunction.string(); !str.empty())
            w_t_TextEdit->setHtml(str.c_str());

        polesTable->setMinimumHeight(250);
        polesTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        h_t_TextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        w_t_TextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        auto okButton = new QPushButton("Понятно", this);
        connect(okButton, &QPushButton::clicked, this, &TranFuncDialog::close);

        auto buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(okButton);
        buttonLayout->addStretch(1);

        mainLayout->addWidget(titleLabel);
        mainLayout->addWidget(polesLabel);
        mainLayout->addWidget(polesTable);
        mainLayout->addWidget(h_t_Label);
        mainLayout->addWidget(h_t_TextEdit);
        mainLayout->addWidget(w_t_Label);
        mainLayout->addWidget(w_t_TextEdit);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);
    }

    static inline QColor rootColor(double value) {
        if (value < 0)
            return Qt::black;
        if (value <= 0.5)
            return {
                static_cast<int>(255 * (value * 2)),
                static_cast<int>(255 * (1 - value * 2)),
                0
            };
        return {
            255,
            static_cast<int>(255 * (1 - (value - 0.5) * 2)),
            0
        };
    }

    void formatPoles(QTableWidget* table) const {
        table->setColumnCount(8);
        table->setHorizontalHeaderLabels({
            "Re", "Im", "Abs", "Arg, °", "τ, c", "ζ", "T, c", "Значимость"
        });
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        auto header = table->horizontalHeader();
        header->setToolTip(
            "Re\t— действительная часть;\n"
            "Im\t— мнимая часть;\n"
            "Abs\t— абсолютное значение;\n"
            "Arg\t— угол, фаза (аргумент);\n"
            "τ\t— время релаксации;\n"
            "ζ\t— коэффициент демпфирования;\n"
            "T\t— период колебания."
        );
        header->setSectionsClickable(true);

        int row = -1;
        const auto roots = tf.getRoots();
        const auto n = roots.size();
        for (std::size_t i = 0; i < n; ++i) {
            const auto& pole = roots[i];
            table->insertRow(++row);

            if (pole.real() < 0) {
                table->setItem(row, 0, new QTableWidgetItem((std::stringstream() << pole.real()).str().c_str()));
                table->item(row, 0)->setForeground(QBrush(QColor(Qt::blue)));
            }
            else {
                table->setItem(row, 0, new QTableWidgetItem((std::stringstream() << '+' << pole.real()).str().c_str()));
                table->item(row, 0)->setForeground(QBrush(QColor(Qt::red)));
            }

            if (std::abs(pole.imag()) > 1e-10) {
                table->setItem(row, 1, new QTableWidgetItem((std::stringstream() << "±" << std::abs(pole.imag())).str().c_str()));
                table->setItem(row, 5, new QTableWidgetItem(QString::number(pole.real() != 0 ? -pole.real() / std::abs(pole) : 0, 'f', 4)));
                table->setItem(row, 6, new QTableWidgetItem(QString::number(2 * std::numbers::pi / -pole.imag(), 'f', 4)));
            }

            table->setItem(row, 2, new QTableWidgetItem(QString::number(std::abs(pole), 'f', 4)));
            table->setItem(row, 3, new QTableWidgetItem(QString::number(std::arg(pole) * 180.0 / std::numbers::pi, 'f', 4)));
            table->setItem(row, 4, new QTableWidgetItem(QString::number(pole.real() != 0 ? -1.0 / pole.real() : std::numeric_limits<double>::infinity(), 'f', 4)));

            auto colorItem = new QTableWidgetItem();
            colorItem->setBackground(rootColor(roots[0].real() / roots[i].real()));
            table->setItem(row, 7, colorItem);

            if (i + 1 < n && pole == std::conj(roots[i + 1]))
                ++i;
        }

        for (int i = 0; i <= row; ++i)
            for (int j = 2; j < 7; ++j)
                if (auto item = table->item(i, j); item)
                    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (int i = 0; i <= row; ++i)
            for (int j = 0; j <= 1; ++j)
                if (auto item = table->item(i, j); item)
                    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
};

#endif // REGVALVE_TRAN_FUNC_DIALOG_HPP