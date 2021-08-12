#include <iostream>
#include "pca.h"
#include "eigen.h"
#include <math.h>

using namespace std;


PCA::PCA(unsigned int n_components) {
	selected_components = n_components;
}

void PCA::fit(Matrix images_to_fit) {
	Matrix X(images_to_fit);
	for (int i = 0; i < images_to_fit.cols(); i++) {
		double mean = images_to_fit.col(i).mean();
		Vector means(images_to_fit.rows());
		for (int j = 0; j < images_to_fit.rows(); j++) {
			means(j) = mean;
		}
		X.col(i) = X.col(i) - means;
	}

	Matrix covariances = pow(images_to_fit.cols() - 1, -1) * X.transpose() * X;
	eigen_vectors = (get_first_eigenvalues(covariances, selected_components, 10000, 1e-16)).second;
	fitted_images = images_to_fit * eigen_vectors;
}


MatrixXd PCA::transform(Matrix images_to_predict){
	return images_to_predict * eigen_vectors;
}
