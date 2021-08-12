#include <algorithm>
#include <chrono>
#include <iostream>
#include "eigen.h"

using namespace std;


pair<double, Vector> power_iteration(const Matrix& A, unsigned iterations, double epsilon) {
    Vector eigen_vector = Vector::Random(A.cols());
    eigen_vector = eigen_vector / eigen_vector.norm();
    
    double eigen_value = 0;
    double previous_eigen_value = 0;
    
    for (size_t i = 0; i < iterations; i++) {
        previous_eigen_value = eigen_value;
        eigen_vector = A * eigen_vector;
        eigen_vector = eigen_vector / eigen_vector.norm();
        eigen_value = eigen_vector.transpose() * A * eigen_vector;
        
        // Posibles métodos de corte
        if (i > 0 && (abs(eigen_value - previous_eigen_value) < epsilon)) {break;}
        //if ((A * eigen_vector).isApprox(eigen_value * eigen_vector, epsilon)) {break;}
    }
    
    return make_pair(eigen_value, eigen_vector);
}

pair<Vector, Matrix> get_first_eigenvalues(const Matrix& X, unsigned amount, unsigned iterations, double epsilon) {
    Matrix A(X);
    Vector eigen_values(amount);
    Matrix eigen_vectors(A.rows(), amount);

    for (size_t i = 0; i < amount; i++) {
        pair<double, Vector> result = power_iteration(A, iterations, epsilon);
        eigen_values(i) = result.first;
        eigen_vectors.col(i) = result.second;
        A = A - result.first * result.second * result.second.transpose();
    }
    
    return make_pair(eigen_values, eigen_vectors);
}