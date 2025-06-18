#include "code/qt-application/application.h"

#include <iostream>
#include "code/reg-core.hpp"

#include <chrono>

#include <chrono>
#include <functional>
#include "code/numina/terms/term-expression.hpp"

class TranFunc {
private:
    std::vector <std::complex <double>> roots = {
            {-0.05, -0.5}, {-0.05, 0.5}, {-0.05, -0.5}, {-0.05, 0.5},
            {-0.05, -0.5}, {-0.05, 0.5}, {-0.05, -0.5}, {-0.05, 0.5},
            {-0.05, -0.5}, {-0.05, 0.5}, {-0.05, -0.5}, {-0.05, 0.5},
            {-0.05, -0.5}, {-0.05, 0.5}, {-0.05, -0.5}, {-0.05, 0.5}
    };

    //std::vector <Term <std::complex<double>>*> terms;
    std::vector <Term <double>*> terms;

    std::vector <int> powers = {0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2};
    //std::vector <double> powers = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //std::vector <double> coeffs = {5, 4, 8, 1, 5, 4, 8, 1, 5, 4, 8, 1, 5, 4, 8, 1};
    std::vector <std::complex <double>> coeffs = {5, 4, 8, 1, 5, 4, 8, 1, 5, 4, 8, 1, 5, 4, 8, 1};

public:
    TranFunc() : impulseResp(roots, coeffs, powers)
    {
        /*const std::size_t n = roots.size();
        for (std::size_t i = 0; i < n; i += 2) {
            terms.push_back(createTerm(coeffs[i], roots[i], (int)powers[i]));
        }

        for (auto t : terms)
            std::cout << t->string() << '\n';*/
    }

    TermExpression <double> impulseResp;

    ~TranFunc() {
        for (auto term : terms)
            delete term;
    }

    inline double impulseResponse(double time) {
        std::complex <double> answer = 0;
        const std::size_t n = roots.size();
        for (std::size_t i = 0; i < n; ++i)
            answer += coeffs[i] * std::exp(roots[i] * time);

        return answer.real();
    }

    inline double fullImpulseResponse(double time) {
        std::complex <double> answer = 0;
        const std::size_t n = roots.size();
        for (std::size_t i = 0; i < n; ++i)
            answer += coeffs[i] * std::exp(roots[i]) * std::pow(time, powers[i]);

        return answer.real();
    }

    inline double altImpulseResponse(double time) {
        //std::complex <double> answer = 0;
        double answer = 0;
        //Term<std::complex<double>>::time = time;
        Term<double>::time = time;
        const std::size_t n = terms.size();
        for (std::size_t i = 0; i < n; ++i)
            answer += terms[i]->value();

        return answer;
    }
};

void test(const std::function<double(double)>& h) {
    auto start = std::chrono::high_resolution_clock::now();

    double sum = 0;
    for (int i = 0; i < 1e7; ++i)
        sum += h(i);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "time: " << (end - start).count() / 1e9 << '\n';
    std::cout << "sum = " << sum << '\n';
}

int main(int argc, char* argv[]) {
    /*TranFunc tf;

    test(std::bind(&TermExpression<double>::operator(), &tf.impulseResp, std::placeholders::_1));
    //test(std::bind(&TranFunc::impulseResponse, &tf, std::placeholders::_1));
    //test(std::bind(&TranFunc::fullImpulseResponse, &tf, std::placeholders::_1));

    return 0;*/

    QApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::Russian));
    QTranslator translator;
    if (translator.load("qt_ru", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&translator);

    Application window;
    window.show();
    return QApplication::exec();
}
