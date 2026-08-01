#include "code/widgets/tf-form/tran-func-form.h"
#include "code/widgets/tf-form/tran-func-form-line-edit.hpp"

#include "code/dialogs/tran-func-dialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QMenu>

#include <cmath>

void TranFuncForm::create_label_context_menu(QLabel* label) {
    auto* about_action = new QAction(tr("Подробнее"), label);
    connect(about_action, &QAction::triggered, this, [this] {
        if (!tf_)
            return;
        TranFuncDialog dialog(*tf_, this);
        dialog.exec();
    });
    auto* menu = new QMenu(label);
    menu->addAction(about_action);
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(label, &QLabel::customContextMenuRequested, this, [menu, label](const QPoint& pos) {
        menu->exec(label->mapToGlobal(pos));
    });
}

void TranFuncForm::update_name_label() {
    if (name_label_)
        name_label_->setText(linkName());
}

void TranFuncForm::bindNameLabel(QLabel* label) {
    name_label_ = label;
    for (auto* le : numerator_)
        connect(le, &QLineEdit::editingFinished, this, &TranFuncForm::update_name_label);
    for (auto* le : denominator_)
        connect(le, &QLineEdit::editingFinished, this, &TranFuncForm::update_name_label);
}

QString TranFuncForm::linkName() const {
    switch (id_) {
    case 1 + 1 * 64:
        return tr("Усилительное (безынерционное)");
    case 1 + 2 * 64:
        return tr("Идеальное интегрирующее (астатическое)");
    case 1 + 3 * 64:
        return tr("Инерционное 1-го порядка (апериодическое)");
    case 1 + 5 * 64:
        return tr("Вырожденное колебательное (консервативное)");
    case 1 + 6 * 64:
        return tr("Реальное интегрирующее (инерционное)");
    case 1 + 7 * 64: {
        const double a1 = denominator_[1]->stored;
        const double a2 = denominator_[2]->stored;
        if (a2 > 0.0 && a1 / (2.0 * std::sqrt(a2)) < 1.0)
            return tr("Колебательное");
        return tr("Инерционное 2-го порядка (апериодическое)");
    }
    case 2 + 1 * 64:
        return tr("Идеальное дифференцирующее");
    case 2 + 3 * 64:
        return tr("Инерционное (реальное) дифференцирующее");
    case 4 + 3 * 64:
        return tr("Реальное дифференцирующее 2-го порядка");
    case 3 + 2 * 64:
        return tr("Изодромное");
    case 3 + 1 * 64:
        return tr("Форсирующее");
    case 3 + 3 * 64:
        return tr("Инерционно-форсирующее");
    case 63 + 21 * 64:
        return tr("Пропорционально-дифференциальное 2-го порядка");
    default:
        return tr("Неизвестно");
    }
}
