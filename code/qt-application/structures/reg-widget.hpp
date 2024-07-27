//
// Created by Vadma on 27.07.2024.
//

#ifndef REGVALVE_REG_WIDGET_HPP
#define REGVALVE_REG_WIDGET_HPP

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>

#include <iomanip>
#include <sstream>

class RegulationWidget : public QWidget {
private:
    QCheckBox* regulationCheckBox;
    QLineEdit* timeRegLineEdit;
    QLineEdit* setValueLineEdit;
    QLineEdit* likLineEdit;
    QLineEdit* ikkLineEdit;
    QLineEdit* skoLineEdit;

    static inline QString formatDouble(double value, int precision) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(precision) << value;
        return QString::fromStdString(out.str());
    }

    static void applyDefaultStyle(QGridLayout* layout) {
        const auto rows(layout->rowCount());
        const auto cols(layout->columnCount());

        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; j += 2)
                if (auto label = qobject_cast<QLabel*>(layout->itemAtPosition(i, j)->widget()); label) {
                    label->setStyleSheet("font-size: 14pt;");
                    label->setFixedSize(50, 25);
                }
        for (int i = 0; i < rows; ++i)
            for (int j = 1; j < cols; j += 2)
                if (auto lineEdit = qobject_cast<QLineEdit*>(layout->itemAtPosition(i, j)->widget()); lineEdit) {
                    lineEdit->setStyleSheet("font-size: 14pt;");
                    lineEdit->setFixedSize(100, 25);
                    lineEdit->setReadOnly(true);
                }
    }

public:
    explicit RegulationWidget(QWidget* parent = nullptr) : QWidget(parent) {
        auto layout = new QGridLayout(this);

        regulationCheckBox  = new QCheckBox(this);
        timeRegLineEdit     = new QLineEdit(this);
        setValueLineEdit    = new QLineEdit(this);
        likLineEdit         = new QLineEdit(this);
        ikkLineEdit         = new QLineEdit(this);
        skoLineEdit         = new QLineEdit(this);

        layout->addWidget(new QLabel("a<sub>рег</sub>:", this), 0, 0);
        layout->addWidget(regulationCheckBox, 0, 1);

        layout->addWidget(new QLabel("t<sub>рег</sub>:", this), 1, 0);
        layout->addWidget(timeRegLineEdit, 1, 1);

        layout->addWidget(new QLabel("y<sub>уст</sub>:", this), 2, 0);
        layout->addWidget(setValueLineEdit, 2, 1);

        layout->addWidget(new QLabel("ЛИК:", this), 0, 2);
        layout->addWidget(likLineEdit, 0, 3);

        layout->addWidget(new QLabel("ИКК:", this), 1, 2);
        layout->addWidget(ikkLineEdit, 1, 3);

        layout->addWidget(new QLabel("СКО:", this), 2, 2);
        layout->addWidget(skoLineEdit, 2, 3);

        setLayout(layout);
        applyDefaultStyle(layout);
    }

    void updateValues(bool regulation, double timeReg, double setValue, double lik, double ikk, double sko) {
        regulationCheckBox->setChecked(regulation);
        timeRegLineEdit->setText(formatDouble(timeReg, 2));
        setValueLineEdit->setText(formatDouble(setValue, 2));
        likLineEdit->setText(formatDouble(lik, 4));
        ikkLineEdit->setText(formatDouble(ikk, 4));
        skoLineEdit->setText(formatDouble(sko, 4));
    }
};

#endif //REGVALVE_REG_WIDGET_HPP
