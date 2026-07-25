#pragma once

#include <QRegularExpression>
#include <QString>
#include <QWidget>
#include <type_traits>

class StyleCore {
public:
    template <typename T>
    static void updateStyle(T* widget, const QString& property, const QString& value) {
        static_assert(std::is_base_of_v<QWidget, T>);
        QString style = widget->styleSheet();
        const QString replacement = QString("%1: %2;").arg(property, value);
        const QRegularExpression pattern(QString("%1:\\s*[^;]+;").arg(property));
        if (const auto match = pattern.match(style); match.hasMatch())
            style.replace(match.capturedStart(), match.capturedLength(), replacement);
        else
            style += replacement;
        widget->setStyleSheet(style);
    }
};
