#ifndef MATH_CORE_SOLVE_MATRIX_HPP
#define MATH_CORE_SOLVE_MATRIX_HPP

#include <vector>
#include <cmath>

namespace SupMathCore {
    /// Решение матрицы методом Гаусса-Жордана
    template <typename Type>
    static std::vector<Type> solveMatrixGauss(Type* matrix, std::size_t n) {
        const std::size_t cols(n + 1);
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t maxRow = i;
            for (std::size_t k = i + 1; k < n; ++k)
                if (std::abs(matrix[k * cols + i]) > std::abs(matrix[maxRow * cols + i]))
                    maxRow = k;
            if (maxRow != i)
                for (std::size_t j = 0; j < cols; ++j)
                    std::swap(matrix[i * cols + j], matrix[maxRow * cols + j]);
            const Type pivot = matrix[i * cols + i];
            for (std::size_t j = i; j < cols; ++j)
                matrix[i * cols + j] /= pivot;
            for (std::size_t k = i + 1; k < n; ++k) {
                const Type factor = matrix[k * cols + i];
                for (std::size_t j = i; j < cols; ++j)
                    matrix[k * cols + j] -= factor * matrix[i * cols + j];
            }
        }
        std::vector <Type> answers(n);
        for (int i = n - 1; i >= 0; --i) {
            answers[i] = matrix[i * cols + n];
            for (int j = i + 1; j < n; ++j)
                answers[i] -= matrix[i * cols + j] * answers[j];
        }
        return answers;
    } /// N^3

    /// Решение матрицы методом Гаусса-Жордана
    template <typename Type>
    static std::vector <Type> solveMatrixGaussJordan(Type *matrix, std::size_t n) {
        const std::size_t cols(n + 1);
        for (std::size_t i = 0; i < n; ++i) {
            const Type diagElement(matrix[i * cols + i]);
            for (std::size_t j = 0; j < cols; ++j)
                matrix[i * cols + j] /= diagElement;

            for (std::size_t k = 0; k < i; ++k) {
                const Type factor(matrix[k * cols + i]);
                for (std::size_t j = 0; j < cols; ++j)
                    matrix[k * cols + j] -= factor * matrix[i * cols + j];
            }
            for (std::size_t k = i + 1; k < n; ++k) {
                const Type factor(matrix[k * cols + i]);
                for (std::size_t j = 0; j < cols; ++j)
                    matrix[k * cols + j] -= factor * matrix[i * cols + j];
            }
        }
        std::vector<Type> answers(n);
        for (std::size_t i = 0; i < n; ++i)
            answers[i] = matrix[i * cols + n];
        return answers;
    } /// N^3
}

#endif //MATH_CORE_SOLVE_MATRIX_HPP
