#include "code/dialogs/help-dialog.h"

#include "ui_help-dialog.h"

HelpDialog::HelpDialog(QWidget* parent) : QDialog(parent), ui(new Ui::HelpDialog) {
    ui->setupUi(this);
    ui->textEdit->setHtml(QStringLiteral(
        "<style>p { text-align: justify; text-indent: 20px; margin-bottom: 10px; }</style>"
        "<p><b>ПИД-регулятор</b> — механизм обратной связи для управления системами. "
        "Он корректирует управляющий сигнал в зависимости от отклонения текущего значения от заданного.</p>"
        "<p><b>K<sub>p</sub></b> — сила реакции на ошибку; рост ускоряет систему, но может вызвать колебания.</p>"
        "<p><b>T<sub>u</sub></b> — время интегрирования; уменьшение быстрее убирает статическую ошибку.</p>"
        "<p><b>T<sub>d</sub></b> — время дифференцирования; демпфирует, но усиливает шум при больших значениях.</p>"
        "<p><b>Важно!</b> Настройка обычно итеративна и требует экспериментов.</p>"));
    connect(ui->okButton, &QPushButton::clicked, this, &HelpDialog::close);
}

HelpDialog::~HelpDialog() {
    delete ui;
}
