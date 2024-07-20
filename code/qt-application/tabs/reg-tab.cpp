//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"

QWidget* Application::createRegTab() {
    QLabel *transferFunctionLabel = new QLabel("W<sub>ОУ</sub>(p) = ");
    transferFunctionLabel->setAlignment(Qt::AlignCenter);
    transferFunctionLabel->setStyleSheet("font-size: 24pt;");

    QHBoxLayout *numeratorLayout = new QHBoxLayout;
    QHBoxLayout *denominatorLayout = new QHBoxLayout;

    QDoubleValidator *realNumberValidator = new QDoubleValidator(this);
    realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

    createLineEdit("Числитель:\t", numeratorLayout, realNumberValidator);
    createLineEdit("Знаменатель:\t", denominatorLayout, realNumberValidator);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMidLineWidth(10);

    QVBoxLayout *transferFunctionLayout = new QVBoxLayout();
    transferFunctionLayout->addLayout(numeratorLayout);
    transferFunctionLayout->addWidget(line);
    transferFunctionLayout->addLayout(denominatorLayout);

    QHBoxLayout *uppLayout = new QHBoxLayout;
    uppLayout->addWidget(transferFunctionLabel);
    uppLayout->addLayout(transferFunctionLayout);

    auto parametersLayout = new QVBoxLayout;
    auto sliders(createControllerParameterForms(parametersLayout));

    for (auto slider : sliders) {
        connect(slider, &QSlider::valueChanged, [this, sliders, numeratorLayout, denominatorLayout] {
            auto numeratorData = getLineEditData(numeratorLayout);
            auto denominatorData = getLineEditData(denominatorLayout);

            if (denominatorData.empty())
                return;
            if (numeratorData.size() > denominatorData.size())
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

            TransferFunction W(numeratorData, denominatorData);
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
    connect(calculateButton, &QPushButton::clicked, [this, numeratorLayout, denominatorLayout] {
        auto numeratorData = getLineEditData(numeratorLayout);
        auto denominatorData = getLineEditData(denominatorLayout);

        if (numeratorData.size() > denominatorData.size())
            return;

        TransferFunction W(numeratorData, denominatorData);
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
    resLayout->addLayout(uppLayout);
    resLayout->addLayout(parametersLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addLayout(chartsLayout);

    regTab->setLayout(resLayout);
    return regTab;
}
