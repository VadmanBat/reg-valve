#include "code/charts/response_chart_bank.h"

#include <QAction>
#include <QGridLayout>
#include <QMenu>

ResponseChartBank::ResponseChartBank(QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("ResponseChartBank"));
    grid_ = new QGridLayout(this);
    grid_->setContentsMargins(0, 0, 0, 0);
    grid_->setSpacing(6);

    chartTran_    = new ChartPanel(tr("Переходная h(t)"), tr("t, с"), tr("h(t)"), this);
    chartImpulse_ = new ChartPanel(tr("Импульсная w(t)"), tr("t, с"), tr("w(t)"), this);
    chartNyquist_ = new ChartPanel(tr("КЧХ (годограф)"), tr("Re W(jω)"), tr("Im W(jω)"), this);
    chartAmp_     = new ChartPanel(tr("АЧХ |W(jω)|"), tr("ω, рад/с"), tr("|W|"), this);
    chartPhase_   = new ChartPanel(tr("ФЧХ arg W(jω)"), tr("ω, рад/с"), tr("φ, °"), this);

    rebuildLayout();
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
            next.*field         = on;
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
    // Only rearrange / show-hide. Series already live on every panel.
    rebuildLayout();
    emit visibilityChanged(vis_);
}

void ResponseChartBank::rebuildLayout() {
    while (QLayoutItem* item = grid_->takeAt(0)) {
        if (QWidget* w = item->widget())
            w->setParent(this);
        delete item;
    }
    for (int r = 0; r < 4; ++r)
        grid_->setRowStretch(r, 0);
    for (int c = 0; c < 3; ++c)
        grid_->setColumnStretch(c, 0);

    ChartPanel* panels[] = {chartTran_, chartImpulse_, chartNyquist_, chartAmp_, chartPhase_};
    const bool flags[]   = {vis_.transient, vis_.impulse, vis_.nyquist, vis_.amplitude, vis_.phase};

    std::vector<ChartPanel*> visible;
    visible.reserve(5);
    for (int i = 0; i < 5; ++i) {
        panels[i]->setVisible(flags[i]);
        if (flags[i])
            visible.push_back(panels[i]);
    }

    const int n = static_cast<int>(visible.size());
    if (n == 0)
        return;

    if (n == 1) {
        grid_->addWidget(visible[0], 0, 0);
        grid_->setRowStretch(0, 1);
        grid_->setColumnStretch(0, 1);
    } else if (n == 2) {
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->setRowStretch(0, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
    } else if (n == 3) {
        // 3 in one row
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->addWidget(visible[2], 0, 2);
        grid_->setRowStretch(0, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
        grid_->setColumnStretch(2, 1);
    } else if (n == 4) {
        // 2×2
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        grid_->addWidget(visible[2], 1, 0);
        grid_->addWidget(visible[3], 1, 1);
        grid_->setRowStretch(0, 1);
        grid_->setRowStretch(1, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
    } else {
        // 5: 2 on top (centered over 3 cols), 3 on bottom
        grid_->addWidget(visible[0], 0, 0);
        grid_->addWidget(visible[1], 0, 1);
        // leave col 2 empty on top — stretch all 3 cols so top pair shares space evenly
        // Better: top widgets span: col0 span2 and col2 span1? User asked 2 top 3 bottom.
        // Place top two in cols 0 and 1 with col stretch equal; bottom three cols 0,1,2.
        grid_->addWidget(visible[2], 1, 0);
        grid_->addWidget(visible[3], 1, 1);
        grid_->addWidget(visible[4], 1, 2);
        // Top: put in cols 0 and 1 only — already placed visible[0],[1]
        // Center top pair: use columns 0-1 for top, or span
        // Re-place top across 3 columns with empty margins via stretch on col2 for top only is hard.
        // Clean approach: top row uses col 0 and col 1, bottom uses 0,1,2 — col2 empty on top is fine.
        grid_->setRowStretch(0, 1);
        grid_->setRowStretch(1, 1);
        grid_->setColumnStretch(0, 1);
        grid_->setColumnStretch(1, 1);
        grid_->setColumnStretch(2, 1);
    }
}

void ResponseChartBank::refitAll() {
    if (!tranSeries_.empty())
        chartTran_->fitAxes(tranSeries_.min_x(), tranSeries_.max_x(), tranSeries_.min_y(), tranSeries_.max_y());
    if (!impulseSeries_.empty())
        chartImpulse_->fitAxes(impulseSeries_.min_x(), impulseSeries_.max_x(), impulseSeries_.min_y(),
                               impulseSeries_.max_y());
    if (!nyquistSeries_.empty())
        chartNyquist_->fitAxes(nyquistSeries_.min_x(), nyquistSeries_.max_x(), nyquistSeries_.min_y(),
                               nyquistSeries_.max_y());
    if (!ampSeries_.empty())
        chartAmp_->fitAxes(ampSeries_.min_x(), ampSeries_.max_x(), ampSeries_.min_y(), ampSeries_.max_y());
    if (!phaseSeries_.empty())
        chartPhase_->fitAxes(phaseSeries_.min_x(), phaseSeries_.max_x(), phaseSeries_.min_y(), phaseSeries_.max_y());
}

ResponseChartBank::Batch ResponseChartBank::computeBatch(const numina::TransferFunction& tf, const ModelParam& params,
                                                         const QString& name) {
    Batch b;
    b.name      = name;
    b.transient = tf_builder::transient(tf, params);
    b.impulse   = tf_builder::impulse(tf, params);
    // One numina frequencyResponse(jω) sweep → КЧХ + АЧХ + ФЧХ
    const auto freq = tf_builder::frequencyBundle(tf, params);
    b.nyquist   = freq.nyquist;
    b.amplitude = freq.amplitude;
    b.phase     = freq.phase;
    return b;
}

void ResponseChartBank::pushBatch(Batch b, bool replaceLast) {
    // Always feed all panels (even if hidden) so toggling visibility never rebuilds series.
    if (replaceLast && !history_.empty()) {
        history_.pop_back();
        if (!tranSeries_.empty())
            tranSeries_.pop_back();
        if (!impulseSeries_.empty())
            impulseSeries_.pop_back();
        if (!nyquistSeries_.empty())
            nyquistSeries_.pop_back();
        if (!ampSeries_.empty())
            ampSeries_.pop_back();
        if (!phaseSeries_.empty())
            phaseSeries_.pop_back();
    }

    auto pushReal = [](ChartPanel* panel, SetSeries<Series>& set, const tf_builder::VecPair& data,
                       const QString& name, bool replaceLast) {
        if (replaceLast && panel->curveCount() > 0)
            panel->replaceLastRealCurve(data, name);
        else
            panel->addRealCurve(data, name);
        set.push_back(Series(data));
    };
    auto pushComplex = [](ChartPanel* panel, SetSeries<ComplexSeries>& set, const tf_builder::VecComp& data,
                          const QString& name, bool replaceLast) {
        if (replaceLast && panel->curveCount() > 0)
            panel->replaceLastComplexCurve(data, name);
        else
            panel->addComplexCurve(data, name);
        set.push_back(ComplexSeries(data));
    };

    pushReal(chartTran_, tranSeries_, b.transient, b.name, replaceLast);
    pushReal(chartImpulse_, impulseSeries_, b.impulse, b.name, replaceLast);
    pushComplex(chartNyquist_, nyquistSeries_, b.nyquist, b.name, replaceLast);
    pushReal(chartAmp_, ampSeries_, b.amplitude, b.name, replaceLast);
    pushReal(chartPhase_, phaseSeries_, b.phase, b.name, replaceLast);

    history_.push_back(std::move(b));
    refitAll();
}

void ResponseChartBank::appendFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                     const QString& name) {
    pushBatch(computeBatch(tf, params, name), false);
}

void ResponseChartBank::appendTransientCurve(const chart_utils::VecPair& points, const QString& name) {
    if (points.empty())
        return;
    chartTran_->addRealCurve(points, name);
    tranSeries_.push_back(Series(points));
    chartTran_->fitAxes(tranSeries_.min_x(), tranSeries_.max_x(), tranSeries_.min_y(), tranSeries_.max_y());
}

void ResponseChartBank::replaceLastFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                          const QString& name) {
    if (history_.empty()) {
        appendFromTf(tf, params, name);
        return;
    }
    pushBatch(computeBatch(tf, params, name), true);
}

void ResponseChartBank::clearAll() {
    history_.clear();
    chartTran_->clearCurves();
    chartImpulse_->clearCurves();
    chartNyquist_->clearCurves();
    chartAmp_->clearCurves();
    chartPhase_->clearCurves();
    tranSeries_.clear();
    impulseSeries_.clear();
    nyquistSeries_.clear();
    ampSeries_.clear();
    phaseSeries_.clear();
}
