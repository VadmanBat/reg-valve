#include "code/tabs/rim-tab.h"
#include "ui_rim-tab.h"

RimTab::RimTab(QWidget* parent) : QWidget(parent), ui(new Ui::RimTab) {
    ui->setupUi(this);
}

RimTab::~RimTab() {
    delete ui;
}
