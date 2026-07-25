#pragma once

#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void loadFonts();
    void applyStyles();
    void centerWindow();

    Ui::MainWindow* ui;
};
