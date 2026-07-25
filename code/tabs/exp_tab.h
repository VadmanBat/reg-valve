#pragma once

#include "code/series/complex_series.hpp"
#include "code/series/series.hpp"
#include "code/series/set_series.hpp"

#include <QChart>
#include <QWidget>

namespace Ui {
class ExpTab;
}

class ExpTab : public QWidget {
    Q_OBJECT
public:
    explicit ExpTab(QWidget* parent = nullptr);
    ~ExpTab() override;

private slots:
    void openFile();

private:
    Ui::ExpTab* ui;
    QChart* chartTran_{nullptr};
    QChart* chartFreq_{nullptr};
    SetSeries<Series> tranSeries_;
    SetSeries<ComplexSeries> freqSeries_;
};
