#pragma once

#include <QStyle>
#include <QVariant>
#include <QWidget>

/// Dynamic-property helpers for QSS (no hardcoded colors in C++).
namespace style_util {

inline void setProperty(QWidget* widget, const char* name, const QVariant& value) {
    if (!widget)
        return;
    widget->setProperty(name, value);
    if (QStyle* style = widget->style()) {
        style->unpolish(widget);
        style->polish(widget);
    }
    widget->update();
}

} // namespace style_util

// Legacy name kept for includes that still reference StyleCore (font/property only).
class StyleCore {
public:
    template <typename T>
    static void setProperty(T* widget, const char* name, const QVariant& value) {
        static_assert(std::is_base_of_v<QWidget, T>);
        style_util::setProperty(widget, name, value);
    }
};

#include <type_traits>
