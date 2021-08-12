#include <algorithm>
//#include <chrono>
#include <pybind11/pybind11.h>
#include <iostream>
#include <exception>
#include "linear_regression.h"

using namespace std;
namespace py=pybind11;

LinearRegression::LinearRegression(){

}

void LinearRegression::fit(Matrix A, Matrix b){
    Matrix ata = A.transpose() * A;
    Matrix atb = A.transpose() * b;
    _solution = ata.fullPivHouseholderQr().solve(atb);
}

Vector LinearRegression::see_solution(){
    return _solution;
}

Matrix LinearRegression::predict(Matrix X){
    //auto ret = MatrixXd::Zero(X.rows(), 1);
    Vector ret(X.rows());
    for (long int i = 0; i < X.rows(); i++){
        float predict = predict_one(X.row(i));
        ret[i] = predict;
    }
    return ret;
}

float LinearRegression::predict_one(Vector x){
    return x.dot(_solution);
}