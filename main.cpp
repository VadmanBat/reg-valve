#include "code/qt-application/application.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    GraphWindow window;
    window.show();
    return app.exec();
}
