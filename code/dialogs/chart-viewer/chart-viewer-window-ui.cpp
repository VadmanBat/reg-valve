#include "code/dialogs/chart-dialog.h"
#include "code/dialogs/chart-viewer/chart-viewer-window.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLegend>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStatusBar>
#include <QToolBar>

void ChartViewerWindow::build_toolbar() {
    auto* tb = addToolBar(tr("Навигация"));
    tb->setMovable(false);
    tb->setIconSize(QSize(20, 20));
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    auto* tools = new QActionGroup(this);
    tools->setExclusive(true);

    act_zoom_ = tb->addAction(tr("🔍□  Зум"));
    act_zoom_->setCheckable(true);
    act_zoom_->setChecked(true);
    act_zoom_->setToolTip(tr("Выделение фрагмента (ЛКМ + рамка)\nКолесо — масштаб к курсору"));
    act_zoom_->setShortcut(QKeySequence(tr("Z")));
    tools->addAction(act_zoom_);

    act_pan_ = tb->addAction(tr("✋  Сдвиг"));
    act_pan_->setCheckable(true);
    act_pan_->setToolTip(tr("Панорамирование (ЛКМ или СКМ)"));
    act_pan_->setShortcut(QKeySequence(tr("H")));
    tools->addAction(act_pan_);

    tb->addSeparator();

    act_zoom_in_  = tb->addAction(tr("＋"));
    act_zoom_out_ = tb->addAction(tr("−"));
    act_home_     = tb->addAction(tr("⌂  Исходный"));
    act_zoom_in_->setToolTip(tr("Приблизить (+)"));
    act_zoom_out_->setToolTip(tr("Отдалить (−)"));
    act_home_->setToolTip(tr("Сбросить масштаб (Home / двойной клик)"));
    act_home_->setShortcut(QKeySequence(Qt::Key_Home));
    act_zoom_in_->setShortcut(QKeySequence(Qt::Key_Plus));
    act_zoom_out_->setShortcut(QKeySequence(Qt::Key_Minus));

    tb->addSeparator();

    act_grid_ = tb->addAction(tr("▦  Сетка"));
    act_grid_->setCheckable(true);
    act_grid_->setChecked(true);
    act_grid_->setToolTip(tr("Показать/скрыть сетку"));
    act_grid_->setShortcut(QKeySequence(tr("G")));

    act_legend_ = tb->addAction(tr("☰  Легенда"));
    act_legend_->setCheckable(true);
    act_legend_->setChecked(chart_->legend() && chart_->legend()->isVisible());
    act_legend_->setToolTip(tr("Показать/скрыть легенду"));
    act_legend_->setShortcut(QKeySequence(tr("L")));

    tb->addSeparator();

    act_save_  = tb->addAction(tr("💾  PNG"));
    act_copy_  = tb->addAction(tr("📋  Копировать"));
    act_props_ = tb->addAction(tr("⚙  Свойства"));
    act_save_->setToolTip(tr("Сохранить как PNG"));
    act_copy_->setToolTip(tr("Копировать изображение в буфер"));
    act_props_->setToolTip(tr("Цвета, толщины, подписи"));

    tb->addSeparator();

    act_fullscreen_ = tb->addAction(tr("⛶  На весь экран"));
    act_fullscreen_->setCheckable(true);
    act_fullscreen_->setShortcut(QKeySequence(Qt::Key_F11));
    act_fullscreen_->setToolTip(tr("Полноэкранный режим (F11)"));

    act_close_ = tb->addAction(tr("✕"));
    act_close_->setToolTip(tr("Закрыть (Esc)"));
    act_close_->setShortcut(QKeySequence(Qt::Key_Escape));

    connect(act_zoom_, &QAction::triggered, this,
            [this] { view_->setTool(chart_viewer::InteractiveChartView::Tool::ZoomRect); });
    connect(act_pan_, &QAction::triggered, this,
            [this] { view_->setTool(chart_viewer::InteractiveChartView::Tool::Pan); });
    connect(act_zoom_in_, &QAction::triggered, view_, &chart_viewer::InteractiveChartView::zoomInStep);
    connect(act_zoom_out_, &QAction::triggered, view_, &chart_viewer::InteractiveChartView::zoomOutStep);
    connect(act_home_, &QAction::triggered, this, [this] { view_->resetView(home_x_, home_y_); });
    connect(act_grid_, &QAction::toggled, view_, &chart_viewer::InteractiveChartView::setGridVisible);
    connect(act_legend_, &QAction::toggled, this, [this](bool on) {
        if (chart_->legend())
            chart_->legend()->setVisible(on);
    });
    connect(act_save_, &QAction::triggered, this, &ChartViewerWindow::save_png);
    connect(act_copy_, &QAction::triggered, this, &ChartViewerWindow::copy_image);
    connect(act_props_, &QAction::triggered, this, &ChartViewerWindow::open_properties);
    connect(act_fullscreen_, &QAction::toggled, this, &ChartViewerWindow::toggle_fullscreen);
    connect(act_close_, &QAction::triggered, this, &QWidget::close);

    view_->viewport()->installEventFilter(this);
}

void ChartViewerWindow::build_status() {
    coord_label_ = new QLabel(tr("Наведите курсор на график"));
    statusBar()->addWidget(coord_label_, 1);
    statusBar()->addPermanentWidget(
        new QLabel(tr("Зум: рамка / колесо · Сдвиг: H или СКМ · Сброс: Home · F11 — полный экран · Esc — закрыть")));
}

void ChartViewerWindow::setup_shortcuts() {
    addAction(act_zoom_);
    addAction(act_pan_);
    addAction(act_home_);
    addAction(act_grid_);
    addAction(act_legend_);
    addAction(act_fullscreen_);
    addAction(act_close_);
    addAction(act_zoom_in_);
    addAction(act_zoom_out_);
}

void ChartViewerWindow::save_png() {
    const QString path = QFileDialog::getSaveFileName(this, tr("Сохранить график"), chart_->title(),
                                                      tr("Рисунок PNG (*.png);;Все файлы (*)"));
    if (path.isEmpty())
        return;
    if (!view_->grab().save(path, "png"))
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось сохранить PNG."));
}

void ChartViewerWindow::copy_image() {
    QApplication::clipboard()->setImage(view_->grab().toImage());
    statusBar()->showMessage(tr("Изображение скопировано"), 2000);
}

void ChartViewerWindow::open_properties() {
    ChartDialog dialog(chart_, this);
    dialog.exec();
}

void ChartViewerWindow::toggle_fullscreen(bool on) {
    if (on)
        showFullScreen();
    else
        showNormal();
}

void ChartViewerWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (isFullScreen()) {
            act_fullscreen_->setChecked(false);
            event->accept();
            return;
        }
        close();
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

bool ChartViewerWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == view_->viewport() && event->type() == QEvent::MouseButtonDblClick) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            view_->resetView(home_x_, home_y_);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
