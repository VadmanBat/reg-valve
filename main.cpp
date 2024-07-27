#include "code/qt-application/application.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Application window;
    window.show();
    return QApplication::exec();
}
