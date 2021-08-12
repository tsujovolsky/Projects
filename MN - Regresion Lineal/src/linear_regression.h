#pragma once

#include "types.h"

class LinearRegression {
public:
    LinearRegression();

    void fit(Matrix X, Matrix y);

    Matrix predict(Matrix X);

    Vector see_solution();
private:
    Vector _solution;
    float predict_one(Vector x);
};
