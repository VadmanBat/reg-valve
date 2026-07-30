#include "code/app/mainwindow.h"
#include "ui_mainwindow.h"

#include "code/tabs/id-tab.h"
#include "code/tabs/analysis-tab.h"
#include "code/tabs/synthesis-tab.h"
#include "code/tabs/rim-tab.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    loadFonts();
    applyStyles();

    ui->tabWidget->addTab(new IdTab(this), tr("Идентификация"));
    ui->tabWidget->addTab(new AnalysisTab(this), tr("Анализ"));
    ui->tabWidget->addTab(new SynthesisTab(this), tr("Синтез"));
    ui->tabWidget->addTab(new RimTab(this), tr("Настройка РИМ"));

    centerWindow();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadFonts() {
    QFontDatabase::addApplicationFont(QStringLiteral("data/fonts/font-awesome-6-free-solid-900.otf"));
}

void MainWindow::applyStyles() {
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
        // Keep a real point size after QSS (px rules leave pointSize == -1).
        QFont f = qApp->font();
        if (f.pointSize() <= 0)
            f.setPointSize(10);
        qApp->setFont(f);
        return;
    }
}

void MainWindow::centerWindow() {
    if (auto* screen = QGuiApplication::primaryScreen()) {
        const QRect g = screen->geometry();
        setGeometry((g.width() - 1200) / 2, (g.height() - 800) / 2, 1200, 800);
    }
}
