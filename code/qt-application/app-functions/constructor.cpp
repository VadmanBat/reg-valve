#include "../application.h"

void Application::loadFonts() {
    const int font_id   = QFontDatabase::addApplicationFont("data/fonts/font-awesome-6-free-solid-900.otf");
    QString fontFamily  = QFontDatabase::applicationFontFamilies(font_id).at(0);
    font.setFamily(fontFamily);
    font.setPointSize(16);
}

void Application::centerWindow() {
    auto screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        const int window_width = 1200;
        const int window_height = 800;
        setGeometry(
                (screenGeometry.width() - window_width) / 2,
                (screenGeometry.height() - window_height) / 2,
                window_width, window_height
        );
    }
}

Application::Application(QWidget* parent) :
    QWidget(parent), numSize(0), regSize(0),
    regTF(6, 6, "W<sub>ОУ</sub>(p) = ")
{
        loadFonts();

        auto tabWidget = new QTabWidget(this);
        tabWidget->addTab(createExpTab(), "КЧХ по h(t)");
        tabWidget->addTab(createNumTab(), "КЧХ по W(p)");
        tabWidget->addTab(createRegTab(), "Ручная настройка регулятора");

        mainLayout = new QStackedLayout(this);
        mainLayout->addWidget(tabWidget);

        setLayout(mainLayout);
        centerWindow();
        applyStyles();
}