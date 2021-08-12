#include <algorithm>
//#include <chrono>
#include <iostream>
#include "knn.h"

using namespace std;


KNNClassifier::KNNClassifier(unsigned int n_neighbors){
    neighbors_amount = n_neighbors;
}

void KNNClassifier::fit(Matrix all_images, Matrix all_labels){
    images = all_images;
    labels = all_labels;
}

Vector KNNClassifier::predict(Matrix images_to_predict){
    unsigned int amount = images_to_predict.rows();
    auto predictions = Vector(amount);

    for (unsigned k = 0; k < amount; ++k){
        predictions(k) = predict_one(images_to_predict.row(k));
    }

    return predictions;
}

int KNNClassifier::predict_one(Matrix image_to_predict){

    vector<tuple<double, int>> distances(images.rows());

    for (unsigned k = 0; k < images.rows(); ++k){
        get<0>(distances[k]) = (image_to_predict - images.row(k)).squaredNorm();
        get<1>(distances[k]) = labels(k);
    }

    sort(distances.begin(), distances.end());
    distances.resize(neighbors_amount);
    
    vector<unsigned int> count(10, 0);
    for (unsigned k = 0; k < distances.size(); ++k){
        unsigned int index = get<1>(distances[k]);
        count[index]++;
    }

    return distance(count.begin(), max_element(count.begin(), count.end()));
}