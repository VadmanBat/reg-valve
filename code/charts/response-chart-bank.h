#pragma once

#include "code/charts/chart-panel.h"
#include "code/model/model-param.hpp"
#include "code/series/complex-series.hpp"
#include "code/series/series.hpp"
#include "code/series/set-series.hpp"
#include "code/util/tf-builder.hpp"

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

#include <vector>

class QGridLayout;
class QMenu;

/// Which response plots are shown (Analysis / Synthesis).
struct ChartVisibility {
    bool transient = true;
    bool impulse   = false;
    bool nyquist   = true;
    bool amplitude = false;
    bool phase     = false;

    [[nodiscard]] int count() const {
        return static_cast<int>(transient) + static_cast<int>(impulse) + static_cast<int>(nyquist) +
               static_cast<int>(amplitude) + static_cast<int>(phase);
    }
};

/// Multi-plot host: h(t), w(t), Nyquist, АЧХ, ФЧХ + visibility menu.
class ResponseChartBank : public QWidget {
    Q_OBJECT
public:
    explicit ResponseChartBank(QWidget* parent = nullptr);

    [[nodiscard]] ChartVisibility visibility() const { return vis_; }
    void setVisibility(ChartVisibility vis);

    /// Build checkable actions into menu (owned by caller or bank).
    void populateMenu(QMenu* menu);

    void appendFromTf(const numina::TransferFunction& tf, const ModelParam& params, const QString& name);
    void replaceLastFromTf(const numina::TransferFunction& tf, const ModelParam& params, const QString& name);

    /// Overlay experimental h(t) on the transient chart only (for identification comparison).
    void appendTransientCurve(const chart_utils::VecPair& points, const QString& name);

    void clearAll();

    [[nodiscard]] bool empty() const { return history_.empty(); }

signals:
    void visibilityChanged(ChartVisibility vis);

private:
    struct Batch {
        QString name;
        tf_builder::VecPair transient;
        tf_builder::VecPair impulse;
        tf_builder::VecComp nyquist;
        tf_builder::VecPair amplitude;
        tf_builder::VecPair phase;
    };

    void rebuildLayout();
    void refitAll();
    void pushBatch(Batch b, bool replaceLast);
    static Batch computeBatch(const numina::TransferFunction& tf, const ModelParam& params, const QString& name);

    ChartVisibility vis_{};
    QGridLayout* grid_{nullptr};

    ChartPanel* chartTran_{nullptr};
    ChartPanel* chartImpulse_{nullptr};
    ChartPanel* chartNyquist_{nullptr};
    ChartPanel* chartAmp_{nullptr};
    ChartPanel* chartPhase_{nullptr};

    SetSeries<Series> tranSeries_;
    SetSeries<Series> impulseSeries_;
    SetSeries<ComplexSeries> nyquistSeries_;
    SetSeries<Series> ampSeries_;
    SetSeries<Series> phaseSeries_;

    std::vector<Batch> history_;
};
