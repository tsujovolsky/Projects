#pragma once
#include "types.h"

class PCA {
    public:
        PCA(unsigned int n_components);

        void fit(Matrix images);

        Eigen::MatrixXd transform(Matrix images);
    private:
        unsigned int selected_components;
        Matrix eigen_vectors;
        Matrix fitted_images;
};
