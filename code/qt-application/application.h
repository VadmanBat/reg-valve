//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_APPLICATION_H
#define REGVALVE_APPLICATION_H

#include <QWidget>
#include <QtCharts>

#include "code/structures/forms/transfer-function-form.hpp"

#include "code/structures/reg-parameter.hpp"
#include "code/structures/reg-widget.hpp"
#include "code/structures/model-param.hpp"

#include "../transfer-function/transfer-function.hpp"

#include "../series/series.hpp"
#include "../series/complex-series.hpp"
#include "../series/set-series.hpp"

class Application : public QWidget {
private:
    using ChartData = std::tuple <QChart*, QString, QString, QString>;
    using ChartsDataset = std::vector <ChartData>;

    using Pair      = std::pair <double, double>;
    using VecPair   = std::vector <Pair>;
    using VecComp   = std::vector <ProjectTypes::Complex>;

    static void showError(const QString& errorMessage);

    /// charts functions:
    static QLayout* createCharts(const ChartsDataset& charts, QWidget* tab);
    static void createChartContextMenu(QChartView* chartView);
    static void createAxes(QChart* chart, const QString &titleX, const QString &titleY);
    static void eraseLastSeries(QChart* chart);
    static void removeAllSeries(QChart* chart);
    static Pair computeAxesRange(double min, double max);
    static void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y);
    static bool saveChartToFile(const QString& fileName, QChart* chart);
    static VecPair readVectorFromFile(const QString& fileName);
    static void addHorLine(QChart* chart, qreal value, const QPen& pen = QPen(Qt::black, 2));
    static void addVerLine(QChart* chart, qreal value, const QPen& pen = QPen(Qt::black, 2));

    template <class Points>
    static void addPoints(QChart* chart, const Points& points, const QString& title, std::size_t index = 0) {
        auto series = new QLineSeries;
        series->setName(title);
        series->setPen(pens[index % 6]);
        for (const auto& [x, y] : points)
            series->append(x, y);
        chart->addSeries(series);
    }
    template <class Points>
    static void addComplexPoints(QChart* chart, const Points& points, const QString& title, std::size_t index = 0) {
        auto series = new QLineSeries;
        series->setName(title);
        series->setPen(pens[index % 6]);
        for (const auto& point : points)
            series->append(point.real(), point.imag());
        chart->addSeries(series);
    }

    inline static QPen pens[6];
    static bool numIsValidInput(const MathCore::Vec &num, const MathCore::Vec &den);

    QWidget* createExpTab();
    QWidget* createNumTab();
    QWidget* createRegTab();

    QFont font;
    QStackedLayout* mainLayout;

/// app-functions:
    void applyStyles();
    void loadFonts();
    void centerWindow();

public:
    explicit Application(QWidget* parent = nullptr);

private slots:
    static void showModParDialog(ModelParam& params);

/// exp-tab:
    void expOpenFile();
/// num-tab:
    void numAddTransferFunction();
    void numReplaceTransferFunction();
    void numClearCharts();
/// reg-tab:
    void regAddTransferFunction();
    void regReplaceTransferFunction();
    void regClearCharts();

private:
    std::size_t numSize, regSize;

    QChart *expChartTranResp{new QChart}, *expChartFreqResp{new QChart};
    QChart *numChartTranResp{new QChart}, *numChartFreqResp{new QChart};
    QChart *regChartTranResp{new QChart}, *regChartFreqResp{new QChart};

    SetSeries <Series> expTranRespSeries, numTranRespSeries, regTranRespSeries;
    SetSeries <ComplexSeries> expFreqRespSeries, numFreqRespSeries, regFreqRespSeries;

    ModelParam numModelParam, regModelParam;
    static VecPair getTranResp(const TransferFunction& W, const ModelParam& params);
    static VecComp getFreqResp(const TransferFunction& W, const ModelParam& params);
    [[nodiscard]] static TransferFunction getTransferFunction(const TransferFunctionForm& form);
    [[nodiscard]] static TransferFunction getRegTransferFunction(const TransferFunctionForm& form, std::vector <double> a, std::vector <double> b);

    const ChartsDataset EXP_CHARTS = {
            {expChartTranResp, "Переходная характеристика", "Время t, секунды", "h(t)"},
            {expChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
    const ChartsDataset NUM_CHARTS = {
            {numChartTranResp, "Переходная характеристика", "Время t, секунды", "Параметр h(t), [ед.изм.] / %ХРО"},
            {numChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };
    const ChartsDataset REG_CHARTS = {
            {regChartTranResp, "Переходный процесс", "Время t, секунды", "h(t)"},
            {regChartFreqResp, "Комплексно-частотная характеристика (КЧХ)", "Реальная ось", "Мнимая ось"}
    };

    TransferFunctionForm numTF, regTF;

    std::vector <RegParameter*> regParameters;

    RegulationWidget* numWidget{new RegulationWidget(3, 2)};
    RegulationWidget* regWidget{new RegulationWidget(3, 4)};
};

#endif //REGVALVE_APPLICATION_H
