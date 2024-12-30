//
// Created by Vadma on 27.12.2024.
//

#ifndef REGVALVE_HELP_WINDOW_HPP
#define REGVALVE_HELP_WINDOW_HPP

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QFont>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

class HelpWindow : public QDialog {
    Q_OBJECT

public:
    explicit HelpWindow(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Справка по настройке ПИД-регулятора");
        setWindowIcon(QIcon::fromTheme("dialog-information"));
        setGeometry(300, 300, 800, 600);
        initUI();
    }

private:
    void initUI() {
        auto mainLayout = new QVBoxLayout(this);

        auto titleLabel = new QLabel("Краткое описание параметров ПИД-регулятора", this);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        auto textEdit = new QTextEdit(this);
        textEdit->setReadOnly(true);
        textEdit->setStyleSheet("font-size: 14px;");
        textEdit->setHtml(
                "<style>"
                "p { text-align: justify; text-indent: 20px; margin-bottom: 10px; }"
                "</style>"
                "<p><b>ПИД-регулятор</b> — это механизм обратной связи, который используется для управления различными системами. "
                "Он корректирует управляющий сигнал в зависимости от отклонения текущего значения от заданного. "
                "Настройка параметров ПИД-регулятора является ключевым шагом для достижения оптимальной производительности.</p>"
                "<p><b>Краткое описание параметров:</b></p>"
                "<p><b>K<sub>p</sub> (пропорциональный коэффициент)</b>: <br>"
                " - определяет <b>силу</b> реакции регулятора на текущую ошибку;<br>"
                " - увеличение K<sub>p</sub> делает систему более <b>быстрой</b>, но может вызвать <b>колебания</b> и <b>перерегулирование</b>;<br>"
                " - слишком малое K<sub>p</sub> делает систему <b>медленной</b> и <b>неэффективной</b>.</p>"
                "<p><b>T<sub>u</sub> [сек] (время интегрирования)</b>: <br>"
                " - определяет <b>скорость</b> устранения <b>статической ошибки</b> (устойчивого отклонения);<br>"
                " - уменьшение T<sub>u</sub> делает регулятор <b>быстрее</b> устраняет ошибку, но может вызвать <b>нестабильность</b>;<br>"
                " - слишком большое T<sub>u</sub> делает регулятор <b>медленным</b> в устранении статической ошибки.</p>"
                "<p><b>T<sub>d</sub> [сек] (время дифференцирования)</b>: <br>"
                " - реагирует на <b>скорость изменения</b> ошибки, <b>предсказывая</b> её дальнейшее поведение;<br>"
                " - увеличение T<sub>d</sub> помогает <b>стабилизировать</b> систему, но делает реакцию <b>менее чувствительной</b>;<br>"
                " - слишком большое T<sub>d</sub> может вызвать <b>шумы</b>.</p>"
                "<p><b>Важно!</b> Настройка ПИД-регулятора часто является <b>итеративным процессом</b> и требует <b>экспериментов</b> для достижения оптимальных значений.</p>"
        );

        mainLayout->addWidget(textEdit);

        auto okButton = new QPushButton("Ок", this);
        connect(okButton, &QPushButton::clicked, this, &HelpWindow::close);

        auto buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(okButton);
        buttonLayout->addStretch(1);
        mainLayout->addLayout(buttonLayout);
        setLayout(mainLayout);
    }
};

#endif //REGVALVE_HELP_WINDOW_HPP
