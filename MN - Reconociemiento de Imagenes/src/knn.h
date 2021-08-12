#pragma once

#include "types.h"


class KNNClassifier {
	public:
		KNNClassifier(unsigned int n_neighbors);

		void fit(Matrix all_images, Matrix all_labels);

		Vector predict(Matrix images_to_predict);		
	private:
		unsigned int neighbors_amount;
		Matrix images;
		Matrix labels;

		int predict_one(Matrix image_to_predict);
};
