#include "transfer-function.hpp"
#include "code/numina/polynomial/solve.hpp"

TransferFunction::TransferFunction(const Vec& numerator, const Vec& denominator) :
        numerator(numerator), denominator(denominator)
{
    roots = numina::solve_polynomial_laguerre(TransferFunction::denominator);
    recomputeBackState();
}

TransferFunction::TransferFunction(Vec&& numerator, Vec&& denominator) :
        numerator(std::move(numerator)), denominator(std::move(denominator))
{
    roots = numina::solve_polynomial_laguerre(TransferFunction::denominator);
    recomputeBackState();
}

TransferFunction::TransferFunction(const Vec& num, const Vec& den, double tau, int order) :
        numerator(num), denominator(den)
{
    roots = numina::solve_polynomial_laguerre(denominator);
    const auto [numPade, denPade] = RegCore::getPade(tau, order);
    auto PadeRoots = numina::solve_polynomial_laguerre(denPade);
    roots.insert(roots.end(), PadeRoots.begin(), PadeRoots.end());
    numerator = MathCore::multiply(numerator, numPade);
    denominator = MathCore::multiply(denominator, denPade);
    recomputeBackState();
}

TransferFunction::TransferFunction(Vec&& num, Vec&& den, double tau, int order) :
        numerator(std::move(num)), denominator(std::move(den))
{
    roots = numina::solve_polynomial_laguerre(denominator);
    const auto [numPade, denPade] = RegCore::getPade(tau, order);
    auto PadeRoots = numina::solve_polynomial_laguerre(denPade);
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