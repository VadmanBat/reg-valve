//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

const char* redSliderStyle =
        R"(
    QSlider::groove:horizontal {
        border: 1px solid #ff4d4d;
        height: 8px;
        background: #ffe6e6;
        margin: 2px 0;
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        background: #ff6666;
        border: 1px solid #ff4d4d;
        width: 18px;
        height: 18px;
        border-radius: 9px;
        margin: -5px 0;
    }
    QSlider::sub-page:horizontal {
        background: #ff9999;
        border: 1px solid #ff4d4d;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::add-page:horizontal {
        background: #ffe6e6;
        border: 1px solid #ff4d4d;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::handle:horizontal:disabled {
        background: #ffcccc;
        border: 1px solid #ff4d4d;
    }
    QSlider::groove:horizontal:disabled {
        background: #fff2f2;
    }
    QSlider::sub-page:horizontal:disabled {
        background: #ffe6e6;
    }
    QSlider::add-page:horizontal:disabled {
        background: #fff2f2;
    }
    QSlider {
        margin: 10px;
    }
)";

const char* greenSliderStyle =
        R"(
    QSlider::groove:horizontal {
        border: 1px solid #66cc66;
        height: 8px;
        background: #e6ffe6;
        margin: 2px 0;
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        background: #66ff66;
        border: 1px solid #66cc66;
        width: 18px;
        height: 18px;
        border-radius: 9px;
        margin: -5px 0;
    }
    QSlider::sub-page:horizontal {
        background: #99ff99;
        border: 1px solid #66cc66;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::add-page:horizontal {
        background: #e6ffe6;
        border: 1px solid #66cc66;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::handle:horizontal:disabled {
        background: #ccffcc;
        border: 1px solid #66cc66;
    }
    QSlider::groove:horizontal:disabled {
        background: #f2fff2;
    }
    QSlider::sub-page:horizontal:disabled {
        background: #e6ffe6;
    }
    QSlider::add-page:horizontal:disabled {
        background: #f2fff2;
    }
    QSlider {
        margin: 10px;
    }
)";

const char* blueSliderStyle =
        R"(
    QSlider::groove:horizontal {
        border: 1px solid #6666cc;
        height: 8px;
        background: #e6e6ff;
        margin: 2px 0;
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        background: #6666ff;
        border: 1px solid #6666cc;
        width: 18px;
        height: 18px;
        border-radius: 9px;
        margin: -5px 0;
    }
    QSlider::sub-page:horizontal {
        background: #9999ff;
        border: 1px solid #6666cc;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::add-page:horizontal {
        background: #e6e6ff;
        border: 1px solid #6666cc;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::handle:horizontal:disabled {
        background: #ccccff;
        border: 1px solid #6666cc;
    }
    QSlider::groove:horizontal:disabled {
        background: #f2f2ff;
    }
    QSlider::sub-page:horizontal:disabled {
        background: #e6e6ff;
    }
    QSlider::add-page:horizontal:disabled {
        background: #f2f2ff;
    }
    QSlider {
        margin: 10px;
    }
)";

const char* checkBoxStyle =
        R"(
    QCheckBox::indicator {
        width: 20;
        height: 20px;
    }
    QCheckBox {
        font-size: 16px;
    }
)";

QColor colors[6] = {
        "#FF0000", /// Красный
        "#003366", /// Синий
        "#009933", /// Зелёный
        "#9900CC", /// Фиолетовый
        "#FF8C00", /// Оранжевый
        "#3399CC"  /// Голубой
};

void Application::applyStyles() {
    regParameters[0]->getSlider()->setStyleSheet(redSliderStyle);
    regParameters[1]->getSlider()->setStyleSheet(greenSliderStyle);
    regParameters[2]->getSlider()->setStyleSheet(blueSliderStyle);

    regParameters[0]->getCheckBox()->setStyleSheet(checkBoxStyle);
    regParameters[1]->getCheckBox()->setStyleSheet(checkBoxStyle);
    regParameters[2]->getCheckBox()->setStyleSheet(checkBoxStyle);

    for (std::size_t i = 0; i < 6; ++i) {
        pens[i].setColor(colors[i]);
        pens[i].setWidth(2);
        pens[i].setStyle(Qt::SolidLine);
        pens[i].setCapStyle(Qt::RoundCap);
        pens[i].setJoinStyle(Qt::MiterJoin);
        pens[i].setJoinStyle(Qt::RoundJoin);
    }
}