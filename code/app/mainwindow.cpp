#include "code/app/mainwindow.h"
#include "ui_mainwindow.h"

#include "code/tabs/exp_tab.h"
#include "code/tabs/num_tab.h"
#include "code/tabs/reg_tab.h"

#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    loadFonts();
    applyStyles();

    ui->tabWidget->addTab(new ExpTab(this), tr("КЧХ по h(t)"));
    ui->tabWidget->addTab(new NumTab(this), tr("КЧХ по W(p)"));
    ui->tabWidget->addTab(new RegTab(this), tr("Ручная настройка регулятора"));

    centerWindow();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadFonts() {
    QFontDatabase::addApplicationFont(QStringLiteral("data/fonts/font-awesome-6-free-solid-900.otf"));
}

void MainWindow::applyStyles() {
    QFile qss(QStringLiteral("styles/button-style.qss"));
    if (!qss.exists())
        qss.setFileName(QStringLiteral("data/styles/button-style.qss"));
    if (qss.open(QFile::ReadOnly)) {
        setStyleSheet(QString::fromLatin1(qss.readAll()));
        qss.close();
    }
}

void MainWindow::centerWindow() {
    if (auto* screen = QGuiApplication::primaryScreen()) {
        const QRect g = screen->geometry();
        const int w = 1200, h = 800;
        setGeometry((g.width() - w) / 2, (g.height() - h) / 2, w, h);
    }
}
