//
// Created by Vadim on 28.07.2024.
//

#ifndef REGVALVE_STYLE_CORE_HPP
#define REGVALVE_STYLE_CORE_HPP

#include <QWidget>
#include <QString>
#include <iostream>

#include <QRegularExpression>

class StyleCore {
public:
    template <typename T>
    static void updateStyle(T* widget, const QString& property, const QString& value) {
        static_assert(std::is_base_of<QWidget, T>::value, "T must be a QWidget or derived class");

        QString style = widget->styleSheet();
        QString replacement = QString("%1: %2;").arg(property,value);
        QRegularExpression pattern(QString("%1:\\s*[^;]+;").arg(property));
        QRegularExpressionMatch match;

        if ((match = pattern.match(style)).hasMatch())
            style.replace(match.capturedStart(), match.capturedLength(), replacement);
        else
            style += replacement;

        widget->setStyleSheet(style);
    }
    template <typename T>
    static void updateStyleSheetProperty(T* widget, const QString& selector, const QString& property, const QString& value) {
        static_assert(std::is_base_of<QWidget, T>::value, "T must be a QWidget or derived class");

        QString style = widget->styleSheet();
        QString replacement = QString("%1: %2;").arg(property, value);
        QString pattern = QString(R"((%1\s*\{[^}]*)(%2\s*:\s*[^;]+;)([^}]*\}))").arg(selector, property);

        QRegularExpression regex(pattern);
        QRegularExpressionMatch match;
        if ((match = regex.match(style)).hasMatch()) {
            style.replace(match.capturedStart(), match.capturedLength(), QString("\\1%1\\3").arg(replacement));
        } else {
            QString selectorPattern = QString(R"((%1\s*\{[^}]*)(\}))").arg(selector);
            QRegularExpression selectorRegex(selectorPattern);
            if ((match = selectorRegex.match(style)).hasMatch()) {
                style.replace(match.capturedStart(), match.capturedLength(), QString("\\1 %2 \\2").arg(replacement));
            } else {
                style += QString("%1 { %2 }").arg(selector, replacement);
            }
        }

        widget->setStyleSheet(style);
    }
};

#endif //REGVALVE_STYLE_CORE_HPP
