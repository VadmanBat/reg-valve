#include "code/charts/response-chart-bank.h"

#include <QAction>
#include <QGridLayout>
#include <QMenu>

ResponseChartBank::ResponseChartBank(QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("ResponseChartBank"));
    grid_ = new QGridLayout(this);
    grid_->setContentsMargins(0, 0, 0, 0);
    grid_->setSpacing(6);

    chart_tran_    = new ChartPanel(tr("Переходная h(t)"), tr("t, с"), tr("h(t)"), this);
    chart_impulse_ = new ChartPanel(tr("Импульсная w(t)"), tr("t, с"), tr("w(t)"), this);
    chart_nyquist_ = new ChartPanel(tr("КЧХ (годограф)"), tr("Re W(jω)"), tr("Im W(jω)"), this);
    chart_amp_     = new ChartPanel(tr("АЧХ |W(jω)|"), tr("ω, рад/с"), tr("|W|"), this);
    chart_phase_   = new ChartPanel(tr("ФЧХ arg W(jω)"), tr("ω, рад/с"), tr("φ, °"), this);

    rebuild_layout();
}

void ResponseChartBank::populateMenu(QMenu* menu) {
    if (!menu)
        return;
    menu->clear();

    auto add = [this, menu](const QString& title, bool ChartVisibility::*field) {
        auto* act = menu->addAction(title);
        act->setCheckable(true);
        act->setChecked(vis_.*field);
        connect(act, &QAction::toggled, this, [this, field, act](bool on) {
            ChartVisibility next = vis_;
            next.*field          = on;
            if (next.count() == 0) {
                act->blockSignals(true);
                act->setChecked(true);
                act->blockSignals(false);
                return;
            }
            setVisibility(next);
        });
    };

    add(tr("Переходная h(t)"), &ChartVisibility::transient);
    add(tr("Импульсная w(t)"), &ChartVisibility::impulse);
    add(tr("КЧХ (Nyquist)"), &ChartVisibility::nyquist);
    add(tr("АЧХ |W(jω)|"), &ChartVisibility::amplitude);
    add(tr("ФЧХ φ(ω)"), &ChartVisibility::phase);
}

void ResponseChartBank::setVisibility(ChartVisibility vis) {
    if (vis.count() == 0)
        vis.transient = true;
    vis_ = vis;
    rebuild_layout();
}

void ResponseChartBank::rebuild_layout() {
    while (QLayoutItem* item = grid_->takeAt(0)) {
        if (QWidget* w = item->widget())
            w->setParent(this);
        delete item;
    }
    for (int r = 0; r < 4; ++r)
        grid_->setRowStretch(r, 0);
    for (int c = 0; c < 3; ++c)
        grid_->setColumnStretch(c, 0);

    ChartPanel* panels[] = {chart_tran_, chart_impulse_, chart_nyquist_, chart_amp_, chart_phase_};
    const bool flags[]   = {vis_.transient, vis_.impulse, vis_.nyquist, vis_.amplitude, vis_.phase};

    ChartPanel* visible[5];
    int n = 0;
    for (int i = 0; i < 5; ++i) {
        panels[i]->setVisible(flags[i]);
        if (flags[i])
            visible[n++] = panels[i];
    }
    if (n == 0)
        return;

    switch (n) {
    case 1:
        grid_->addWidget(visible[0], 0, 0);
        grid_->setRowStretch(0, 1);
        grid_->setColumnStretch(0, 1);
        break;
    case 2:
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->setRowStretch(0, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
        break;
    case 3:
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->addWidget(visible[2], 0, 2);
        grid_->setRowStretch(0, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
        grid_->setColumnStretch(2, 1);
        break;
    case 4:
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->addWidget(visible[2], 1, 0);
        grid_->addWidget(visible[3], 1, 1);
        grid_->setRowStretch(0, 1);
        grid_->setRowStretch(1, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
        break;
    default:
        // 5: 2 top + 3 bottom
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->addWidget(visible[2], 1, 0);
        grid_->addWidget(visible[3], 1, 1);
        grid_->addWidget(visible[4], 1, 2);
        grid_->setRowStretch(0, 1);
        grid_->setRowStretch(1, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
        grid_->setColumnStretch(2, 1);
        break;
    }
}

void ResponseChartBank::refit_all() {
    if (!tran_bounds_.empty())
        chart_tran_->fitAxes(tran_bounds_.min_x(), tran_bounds_.max_x(), tran_bounds_.min_y(), tran_bounds_.max_y());
    if (!impulse_bounds_.empty())
        chart_impulse_->fitAxes(impulse_bounds_.min_x(), impulse_bounds_.max_x(), impulse_bounds_.min_y(),
                                impulse_bounds_.max_y());
    if (!nyquist_bounds_.empty())
        chart_nyquist_->fitAxes(nyquist_bounds_.min_x(), nyquist_bounds_.max_x(), nyquist_bounds_.min_y(),
                                nyquist_bounds_.max_y());
    if (!amp_bounds_.empty())
        chart_amp_->fitAxes(amp_bounds_.min_x(), amp_bounds_.max_x(), amp_bounds_.min_y(), amp_bounds_.max_y());
    if (!phase_bounds_.empty())
        chart_phase_->fitAxes(phase_bounds_.min_x(), phase_bounds_.max_x(), phase_bounds_.min_y(),
                              phase_bounds_.max_y());
}

ResponseChartBank::Batch ResponseChartBank::compute_batch(const numina::TransferFunction& tf,
                                                          const ModelParam& params, const QString& name) {
    Batch b;
    b.name      = name;
    b.transient = tf_builder::transient(tf, params);
    b.impulse   = tf_builder::impulse(tf, params);
    const auto freq = tf_builder::frequencyBundle(tf, params);
    b.nyquist   = std::move(freq.nyquist);
    b.amplitude = std::move(freq.amplitude);
    b.phase     = std::move(freq.phase);
    return b;
}

void ResponseChartBank::push_batch(Batch b, bool replace_last) {
    // Always feed all panels (even if hidden) so toggling visibility never rebuilds series.
    if (replace_last && !history_.empty()) {
        history_.pop_back();
        if (!tran_bounds_.empty())
            tran_bounds_.pop_back();
        if (!impulse_bounds_.empty())
            impulse_bounds_.pop_back();
        if (!nyquist_bounds_.empty())
            nyquist_bounds_.pop_back();
        if (!amp_bounds_.empty())
            amp_bounds_.pop_back();
        if (!phase_bounds_.empty())
            phase_bounds_.pop_back();
    }

    auto push_real = [](ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecPair& data, const QString& name,
                        bool replace) {
        if (replace && panel->curveCount() > 0)
            panel->replaceLastRealCurve(data, name);
        else
            panel->addRealCurve(data, name);
        bounds.push_back(boundsOfReal(data));
    };
    auto push_complex = [](ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecComp& data, const QString& name,
                           bool replace) {
        if (replace && panel->curveCount() > 0)
            panel->replaceLastComplexCurve(data, name);
        else
            panel->addComplexCurve(data, name);
        bounds.push_back(boundsOfComplex(data));
    };

    push_real(chart_tran_, tran_bounds_, b.transient, b.name, replace_last);
    push_real(chart_impulse_, impulse_bounds_, b.impulse, b.name, replace_last);
    push_complex(chart_nyquist_, nyquist_bounds_, b.nyquist, b.name, replace_last);
    push_real(chart_amp_, amp_bounds_, b.amplitude, b.name, replace_last);
    push_real(chart_phase_, phase_bounds_, b.phase, b.name, replace_last);

    history_.push_back(std::move(b));
    refit_all();
}

void ResponseChartBank::appendFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                     const QString& name) {
    push_batch(compute_batch(tf, params, name), false);
}

void ResponseChartBank::appendTransientCurve(const chart_utils::VecPair& points, const QString& name) {
    if (points.empty())
        return;
    chart_tran_->addRealCurve(points, name);
    tran_bounds_.push_back(boundsOfReal(points));
    chart_tran_->fitAxes(tran_bounds_.min_x(), tran_bounds_.max_x(), tran_bounds_.min_y(), tran_bounds_.max_y());
}

void ResponseChartBank::replaceLastFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                          const QString& name) {
    if (history_.empty()) {
        appendFromTf(tf, params, name);
        return;
    }
    push_batch(compute_batch(tf, params, name), true);
}

void ResponseChartBank::clearAll() {
    history_.clear();
    chart_tran_->clearCurves();
    chart_impulse_->clearCurves();
    chart_nyquist_->clearCurves();
    chart_amp_->clearCurves();
    chart_phase_->clearCurves();
    tran_bounds_.clear();
    impulse_bounds_.clear();
    nyquist_bounds_.clear();
    amp_bounds_.clear();
    phase_bounds_.clear();
}
