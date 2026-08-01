#pragma once

#include "code/charts/chart-panel.h"
#include "code/model/model-param.hpp"
#include "code/series/axis-bounds.hpp"
#include "code/series/bounds-set.hpp"
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

private:
    struct Batch {
        QString name;
        tf_builder::VecPair transient;
        tf_builder::VecPair impulse;
        tf_builder::VecComp nyquist;
        tf_builder::VecPair amplitude;
        tf_builder::VecPair phase;
    };

    ChartVisibility vis_{};
    QGridLayout* grid_{nullptr};

    ChartPanel* chart_tran_{nullptr};
    ChartPanel* chart_impulse_{nullptr};
    ChartPanel* chart_nyquist_{nullptr};
    ChartPanel* chart_amp_{nullptr};
    ChartPanel* chart_phase_{nullptr};

    BoundsSet tran_bounds_;
    BoundsSet impulse_bounds_;
    BoundsSet nyquist_bounds_;
    BoundsSet amp_bounds_;
    BoundsSet phase_bounds_;

    std::vector<Batch> history_;

    void rebuild_layout();
    void refit_all();
    void push_batch(Batch b, bool replace_last);
    static Batch compute_batch(const numina::TransferFunction& tf, const ModelParam& params, const QString& name);

public:
    explicit ResponseChartBank(QWidget* parent = nullptr);

    /// Override title of the transient chart (e.g. synthesis: «Переходный процесс»).
    void setTransientTitle(const QString& title);

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
};
