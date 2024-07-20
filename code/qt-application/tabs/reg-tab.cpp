//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"

QWidget* Application::createRegTab() {
    auto transferFunctionLayout = createTransferFunctionForm(regNumerator, regDenominator, 6, 6, "W<sub>ОУ</sub>(p) = ");

    auto parametersLayout = new QVBoxLayout;
    auto sliders(createControllerParameterForms(parametersLayout));

    for (auto slider : sliders) {
        connect(slider, &QSlider::valueChanged, [this, sliders] {
            auto numerator = getLineEditData(regNumerator);
            auto denominator = getLineEditData(regDenominator);

            if (denominator.empty())
                return;
            if (numerator.size() > denominator.size())
                return;

            std::vector <double> num, den;

            double Kp = sliders[0]->value();
            double Tu = sliders[1]->value();
            double Td = sliders[2]->value();

            bool f1(sliders[0]->isEnabled()), f2(sliders[1]->isEnabled()), f3(sliders[2]->isEnabled());

            if (!f1)
                Kp = 1;

            switch (f3 * 2 + f2) {
                case 0:
                    num = {Kp};
                    den = {1};
                    break;
                case 1:
                    num = {Kp * Tu, Kp};
                    den = {Tu, 0};
                    break;
                case 2:
                    num = {Kp * Td, Kp};
                    den = {1};
                    break;
                case 3:
                    num = {Kp * Tu * Td, Kp * Tu, Kp};
                    den = {Tu, 0};
                    break;
            }

            TransferFunction W(numerator, denominator);
            TransferFunction regulator(num, den);

            W *= regulator;
            W.closeLoop();

            std::cout << "is settled: " << W.isSettled() << '\n';
            std::cout << "settling time: " << W.settlingTime() << '\n';
            std::cout << "steady state value: " << W.steadyStateValue() << '\n';

            std::cout << "Kp = " << sliders[0]->value() << '\n';
            std::cout << "Tu = " << sliders[1]->value() << '\n';
            std::cout << "Td = " << sliders[2]->value() << '\n';

            removeAllSeries(regChartTranResp);
            removeAllSeries(regChartFreqResp);

            std::stringstream stream;
            stream << "(" << Kp << ", " << Tu << ", " << Td << ")";
            auto string (stream.str());

            Application::addPoints(regChartTranResp, W.isSettled() ? W.transientResponse() : W.transientResponse({0, 1000}), string.c_str());
            Application::addComplexPoints(regChartFreqResp, W.frequencyResponse(), string.c_str());

            updateAxes(regChartTranResp);
            updateAxes(regChartFreqResp);
        });
    }

    QPushButton *calculateButton = new QPushButton("Рассчитать");
    connect(calculateButton, &QPushButton::clicked, [this] {
        auto numerator = getLineEditData(regNumerator);
        auto denominator = getLineEditData(regDenominator);

        if (numerator.size() > denominator.size())
            return;

        TransferFunction W(numerator, denominator);
        W.closeLoop();
        std::cout << "is settled: " << W.isSettled() << '\n';
        std::cout << "settling time: " << W.settlingTime() << '\n';
        std::cout << "steady state value: " << W.steadyStateValue() << '\n';

        removeAllSeries(regChartTranResp);
        removeAllSeries(regChartFreqResp);

        Application::addPoints(regChartTranResp, W.transientResponse(), "Тест");
        Application::addComplexPoints(regChartFreqResp, W.frequencyResponse(), "Тест");

        updateAxes(regChartTranResp);
        updateAxes(regChartFreqResp);
    });

    QWidget *regTab = new QWidget();

    auto chartsLayout = createCharts(REG_CHARTS, regTab);

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(transferFunctionLayout);
    resLayout->addLayout(parametersLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addLayout(chartsLayout);

    regTab->setLayout(resLayout);
    return regTab;
}
