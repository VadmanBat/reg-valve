//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

void Application::expOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Файлы данных (*.txt *.csv)");
    if (!fileName.isEmpty()) {
        // Здесь должен быть ваш код для чтения файла и построения данных
        // Например, вы можете прочитать файл и заполнить QVector<QPointF> points данными для построения графика
        // ...

    }
}
