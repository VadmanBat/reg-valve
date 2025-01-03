//
// Created by Vadma on 03.01.2025.
//
#include "transfer-function.hpp"

TransferFunction::TransferFunction(Vec numerator, Vec denominator) :
        numerator(std::move(numerator)), denominator(std::move(denominator))
{
    roots = SupMathCore::solvePolynomialLaguerre(TransferFunction::denominator);
    recomputeBackState();
}

TransferFunction::TransferFunction(Vec&& numerator, Vec&& denominator) :
        numerator(std::move(numerator)), denominator(std::move(denominator))
{
    roots = SupMathCore::solvePolynomialLaguerre(TransferFunction::denominator);
    recomputeBackState();
}

TransferFunction::TransferFunction(Vec num, Vec den, double tau, int order) :
        numerator(std::move(num)), denominator(std::move(den))
{
    roots = SupMathCore::solvePolynomialLaguerre(denominator);
    const auto [numPade, denPade] = RegCore::getPade(tau, order);
    auto PadeRoots = SupMathCore::solvePolynomialLaguerre(denPade);
    roots.insert(roots.end(), PadeRoots.begin(), PadeRoots.end());
    numerator = MathCore::multiply(numerator, numPade);
    denominator = MathCore::multiply(denominator, denPade);
    recomputeBackState();
}

TransferFunction::TransferFunction(Vec&& num, Vec&& den, double tau, int order) :
        numerator(std::move(num)), denominator(std::move(den))
{
    roots = SupMathCore::solvePolynomialLaguerre(denominator);
    const auto [numPade, denPade] = RegCore::getPade(tau, order);
    auto PadeRoots = SupMathCore::solvePolynomialLaguerre(denPade);
    roots.insert(roots.end(), PadeRoots.begin(), PadeRoots.end());
    numerator = MathCore::multiply(numerator, numPade);
    denominator = MathCore::multiply(denominator, denPade);
    recomputeBackState();
}

TransferFunction::TransferFunction(const Vec& objNum, const Vec& objDen, const Vec& regNum, const Vec& regDen) :
        numerator(MathCore::multiply(regNum, objNum)),
        denominator(MathCore::multiply(regDen, objDen))
{
    closeLoop();
}

TransferFunction::TransferFunction(const Vec& objNum, const Vec& objDen, const Vec& regNum, const Vec& regDen, double tau, int order) :
        numerator(MathCore::multiply(regNum, objNum)),
        denominator(MathCore::multiply(regDen, objDen))
{
    const auto [numPade, denPade] = RegCore::getPade(tau, order);
    numerator = MathCore::multiply(numerator, numPade);
    denominator = MathCore::multiply(denominator, denPade);
    closeLoop();
}