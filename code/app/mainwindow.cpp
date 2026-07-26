#include "code/app/mainwindow.h"
#include "ui_mainwindow.h"

#include "code/tabs/exp_tab.h"
#include "code/tabs/num_tab.h"
#include "code/tabs/reg_tab.h"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    loadFonts();
    applyStyles();

    // 1) data → plant model · 2) open-loop plant behaviour · 3) controller design
    ui->tabWidget->addTab(new ExpTab(this), tr("Идентификация"));
    ui->tabWidget->addTab(new NumTab(this), tr("Анализ"));
    ui->tabWidget->addTab(new RegTab(this), tr("Синтез"));

    centerWindow();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadFonts() {
    QFontDatabase::addApplicationFont(QStringLiteral("data/fonts/font-awesome-6-free-solid-900.otf"));
}

void MainWindow::applyStyles() {
    // Prefer app.qss; fall back to legacy button-style.qss paths used in deploy layouts.
    const QStringList candidates = {
        QStringLiteral("data/styles/app.qss"),
        QStringLiteral("styles/app.qss"),
        QStringLiteral("data/styles/button-style.qss"),
        QStringLiteral("styles/button-style.qss"),
    };
    for (const QString& path : candidates) {
        QFile qss(path);
        if (!qss.open(QFile::ReadOnly))
            continue;
        qApp->setStyleSheet(QString::fromUtf8(qss.readAll()));
        qss.close();
        return;
    }
}

void MainWindow::centerWindow() {
    if (auto* screen = QGuiApplication::primaryScreen()) {
        const QRect g = screen->geometry();
        const int w = 1200, h = 800;
        setGeometry((g.width() - w) / 2, (g.height() - h) / 2, w, h);
    }
}
