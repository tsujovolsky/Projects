#ifndef GAUSSIAN_ELIMINATION_H
#define GAUSSIAN_ELIMINATION_H

#include <vector>

using namespace std;

vector<double> multiplication_by_scalar(double alpha, vector<double> v){
	vector<double> res(v.size());
	for (int i = 0; i < v.size(); ++i) {
	   res[i] = alpha*v[i];
	}
	return res;
}

//v1.size() == v2.size()
vector<double> vector_subtraction(vector<double> v1, vector<double> v2){
	vector<double> res(v1.size());
	for (int i = 0; i < v1.size(); ++i) {
		res[i] = v1[i] - v2[i];
	}
	return res;
}

void gaussian_elimination(vector<vector<double>>& A, vector<double>& b){
	for (int j = 0; j < A.size()-1; ++j) {
		double A_jj = A[j][j];
		
		for (int i = j+1; i < A.size(); ++i) {
			double m_ij = A[i][j] / A_jj;
			A[i] = vector_subtraction(A[i], multiplication_by_scalar(m_ij, A[j]));
			b[i] = b[i] - (m_ij * b[j]);
		}
	}
}

vector<double> linear_equations_system_solver(vector<vector<double>> A, vector<double> b){
	vector<double> X(b.size(), 0);
	for (int i = b.size()-1; i >= 0 ; --i) {
		double Y = 0;
		for (int j = b.size()-1; j > i ; --j) {
			Y = Y + (A[i][j] * X[j]);
		}
		X[i] = (b[i] - Y) / A[i][i];
	}
	return X;
}

#endif
