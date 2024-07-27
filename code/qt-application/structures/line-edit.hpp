//
// Created by Vadma on 20.07.2024.
//

#ifndef REGVALVE_LINE_EDIT_HPP
#define REGVALVE_LINE_EDIT_HPP

#include <QLineEdit>
#include <QFocusEvent>

class LineEdit : public QLineEdit {
public:
    explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

protected:
    void focusOutEvent(QFocusEvent *event) override {
        QLineEdit::focusOutEvent(event);
        emit editingFinished(); // Явно вызываем сигнал editingFinished
    }
};

#endif //REGVALVE_LINE_EDIT_HPP
