#include "code/app/mainwindow.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::Russian));
    QTranslator translator;
    if (translator.load(QStringLiteral("qt_ru"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&translator);

    MainWindow window;
    window.show();
    return QApplication::exec();
}
