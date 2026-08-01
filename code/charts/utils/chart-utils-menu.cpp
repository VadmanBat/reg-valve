#include "code/charts/utils/chart-utils.hpp"

#include "code/dialogs/chart-dialog.h"
#include "code/dialogs/chart-viewer/chart-viewer-window.h"

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>

#define CHART_TR(str) QCoreApplication::translate("chart_utils", str)

namespace chart_utils {

void openChartViewer(QChart* chart, QWidget* parent) {
    ChartViewerWindow::open(chart, parent);
}

void createChartContextMenu(QChartView* chart_view) {
    auto* open_viewer_action = new QAction(CHART_TR("Открыть в окне…"), chart_view);
    auto* save_image_action  = new QAction(CHART_TR("Сохранить как PNG"), chart_view);
    auto* save_text_action   = new QAction(CHART_TR("Сохранить как TXT"), chart_view);
    auto* copy_action        = new QAction(CHART_TR("Копировать изображение"), chart_view);
    auto* properties_action  = new QAction(CHART_TR("Свойства"), chart_view);

    QChart* chart = chart_view->chart();

    QObject::connect(open_viewer_action, &QAction::triggered, [chart, chart_view] {
        openChartViewer(chart, chart_view->window());
    });

    QObject::connect(save_image_action, &QAction::triggered, [chart, chart_view] {
        const QString file_name =
            QFileDialog::getSaveFileName(nullptr, CHART_TR("Сохранить график"), chart->title(),
                                         CHART_TR("Рисунок PNG (*.png);;Все файлы (*)"));
        if (!file_name.isEmpty() && !chart_view->grab().save(file_name, "png"))
            QMessageBox::warning(nullptr, CHART_TR("Ошибка"), CHART_TR("Не удалось сохранить график!"));
    });

    QObject::connect(save_text_action, &QAction::triggered, [chart] {
        const QString file_name =
            QFileDialog::getSaveFileName(nullptr, CHART_TR("Сохранить график"), chart->title(),
                                         CHART_TR("Текст txt (*.txt);;Все файлы (*)"));
        if (!file_name.isEmpty() && !saveChartToFile(file_name, chart))
            QMessageBox::warning(nullptr, CHART_TR("Ошибка"), CHART_TR("Не удалось сохранить график!"));
    });

    QObject::connect(copy_action, &QAction::triggered, [chart_view] {
        QApplication::clipboard()->setImage(chart_view->grab().toImage());
    });

    QObject::connect(properties_action, &QAction::triggered, [chart] {
        ChartDialog dialog(chart);
        dialog.exec();
    });

    auto* context_menu = new QMenu(chart_view);
    context_menu->addAction(open_viewer_action);
    context_menu->addSeparator();
    context_menu->addAction(save_image_action);
    context_menu->addAction(save_text_action);
    context_menu->addAction(copy_action);
    context_menu->addSeparator();
    context_menu->addAction(properties_action);

    chart_view->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(chart_view, &QChartView::customContextMenuRequested,
                     [context_menu, chart_view](const QPoint& pos) {
                         context_menu->exec(chart_view->mapToGlobal(pos));
                     });

    struct DblOpen final : QObject {
        QChartView* view;
        explicit DblOpen(QChartView* v) : QObject(v), view(v) { v->viewport()->installEventFilter(this); }
        bool eventFilter(QObject* obj, QEvent* ev) override {
            if (obj == view->viewport() && ev->type() == QEvent::MouseButtonDblClick) {
                auto* me = static_cast<QMouseEvent*>(ev);
                if (me->button() == Qt::LeftButton) {
                    openChartViewer(view->chart(), view->window());
                    return true;
                }
            }
            return QObject::eventFilter(obj, ev);
        }
    };
    new DblOpen(chart_view);
}

} // namespace chart_utils
