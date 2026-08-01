#include "code/app/mainwindow.h"

#include <QApplication>
#include <QFont>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::Russian));

    QFont app_font = QApplication::font();
    if (app_font.pointSize() <= 0)
        app_font.setPointSize(10);
    QApplication::setFont(app_font);

    QTranslator translator;
    if (translator.load(QStringLiteral("qt_ru"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&translator);

    MainWindow window;
    window.show();
    return QApplication::exec();
}
