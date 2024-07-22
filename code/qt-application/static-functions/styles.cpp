//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

const char* redSliderStyle =
        R"(
    QSlider::groove:horizontal {
        border: 1px solid #ff0000;
        height: 8px;
        background: #ffcccc;
        margin: 2px 0;
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        background: #ff4d4d;
        border: 1px solid #cc0000;
        width: 18px;
        height: 18px;
        border-radius: 9px;
        margin: -5px 0;
    }
    QSlider::sub-page:horizontal {
        background: #ff6666;
        border: 1px solid #cc0000;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::add-page:horizontal {
        background: #ffcccc;
        border: 1px solid #cc0000;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::handle:horizontal:disabled {
        background: #ff9999;
        border: 1px solid #cc0000;
    }
    QSlider::groove:horizontal:disabled {
        background: #ffe6e6;
    }
    QSlider::sub-page:horizontal:disabled {
        background: #ffcccc;
    }
    QSlider::add-page:horizontal:disabled {
        background: #ffe6e6;
    }
    QSlider {
        margin: 10px;
    }
)";

const char* greenSliderStyle =
        R"(
    QSlider::groove:horizontal {
        border: 1px solid #00ff00;
        height: 8px;
        background: #ccffcc;
        margin: 2px 0;
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        background: #33cc33;
        border: 1px solid #00cc00;
        width: 18px;
        height: 18px;
        border-radius: 9px;
        margin: -5px 0;
    }
    QSlider::sub-page:horizontal {
        background: #66ff66;
        border: 1px solid #00cc00;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::add-page:horizontal {
        background: #ccffcc;
        border: 1px solid #00cc00;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::handle:horizontal:disabled {
        background: #99e699;
        border: 1px solid #00cc00;
    }
    QSlider::groove:horizontal:disabled {
        background: #e6ffe6;
    }
    QSlider::sub-page:horizontal:disabled {
        background: #ccffcc;
    }
    QSlider::add-page:horizontal:disabled {
        background: #e6ffe6;
    }
    QSlider {
        margin: 10px;
    }
)";

const char* blueSliderStyle =
        R"(
    QSlider::groove:horizontal {
        border: 1px solid #0000ff;
        height: 8px;
        background: #ccccff;
        margin: 2px 0;
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        background: #4d4dff;
        border: 1px solid #0000cc;
        width: 18px;
        height: 18px;
        border-radius: 9px;
        margin: -5px 0;
    }
    QSlider::sub-page:horizontal {
        background: #6666ff;
        border: 1px solid #0000cc;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::add-page:horizontal {
        background: #ccccff;
        border: 1px solid #0000cc;
        height: 8px;
        border-radius: 4px;
    }
    QSlider::handle:horizontal:disabled {
        background: #9999ff;
        border: 1px solid #0000cc;
    }
    QSlider::groove:horizontal:disabled {
        background: #e6e6ff;
    }
    QSlider::sub-page:horizontal:disabled {
        background: #ccccff;
    }
    QSlider::add-page:horizontal:disabled {
        background: #e6e6ff;
    }
    QSlider {
        margin: 10px;
    }
)";

void Application::applyStyles() {
    regSliders[0]->setStyleSheet(redSliderStyle);
    regSliders[1]->setStyleSheet(greenSliderStyle);
    regSliders[2]->setStyleSheet(blueSliderStyle);
}