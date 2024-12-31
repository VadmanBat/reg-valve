#include "code/qt-application/application.h"

#include <iostream>
#include "code/reg-core.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::Russian));
    QTranslator translator;
    if (translator.load("qt_ru", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&translator);

    Application window;
    window.show();
    return QApplication::exec();
}
