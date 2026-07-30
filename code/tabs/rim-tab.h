#pragma once

#include <QWidget>

namespace Ui {
class RimTab;
}

/// Placeholder: «Настройка РИМ» (планирование).
class RimTab : public QWidget {
    Q_OBJECT
public:
    explicit RimTab(QWidget* parent = nullptr);
    ~RimTab() override;

private:
    Ui::RimTab* ui;
};
