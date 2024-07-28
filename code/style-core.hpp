//
// Created by Vadma on 28.07.2024.
//

#ifndef REGVALVE_STYLE_CORE_HPP
#define REGVALVE_STYLE_CORE_HPP

#include <QWidget>
#include <QString>
#include <iostream>

class StyleCore {
public:
    template <typename T>
    static void updateStyleSheetProperty(T* widget, const QString& property, const QString& value) {
        static_assert(std::is_base_of<QWidget, T>::value, "T must be a QWidget or derived class");

        QString style = widget->styleSheet();
        QString replacement = QString("%1: %2;").arg(property,value);
        QRegExp pattern(QString("%1: [^;]+;").arg(property));

        if (pattern.indexIn(style) != -1)
            style.replace(pattern, replacement);
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

        QRegExp regex(pattern);
        if (regex.indexIn(style) != -1) {
            style.replace(regex, QString("\\1%1\\3").arg(replacement));
        } else {
            QString selectorPattern = QString(R"((%1\s*\{[^}]*)(\}))").arg(selector);
            QRegExp selectorRegex(selectorPattern);
            if (selectorRegex.indexIn(style) != -1) {
                style.replace(selectorRegex, QString("\\1 %2 \\2").arg(replacement));
            } else {
                style += QString("%1 { %2 }").arg(selector, replacement);
            }
        }

        widget->setStyleSheet(style);
    }
};

#endif //REGVALVE_STYLE_CORE_HPP
