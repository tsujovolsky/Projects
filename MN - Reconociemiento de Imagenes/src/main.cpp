//
// Created by pachi on 5/6/19.
//

#include <iostream>
#include <fstream>
#include <string>
#include "knn.h"
#include "pca.h"
#include "eigen.h"

using namespace std;

void print_help() {
  cout << "Usage: tp2 [-m <method_id>] [-i <train_set_path>] [-q <test_set_path>] [-o <classification_output_path>]" << endl << endl;
  cout << " * Optional parameters: [-k <knn_neighbors>    | default:  8]" << endl;
  cout << "                        [-p <pca_components>   | default: 34]" << endl;
  //cout << "                        [-f <kfold_partitions> | default:  5]" << endl;
  cout << endl;
  exit(1);
}

template<typename M>
M load_from(const string & file_path) {
    ifstream input_file;
	  input_file.open(file_path, ios::in);
    string line;

    vector<double> values;
    uint rows = 0;

    // Drop first line (csv header)
    getline(input_file, line);

    while (getline(input_file, line)) {
        stringstream lineStream(line);
        string cell;
        while (getline(lineStream, cell, ',')) {
            values.push_back(stod(cell));
        }
        ++rows;
    }
    return Eigen::Map<const Eigen::Matrix<typename M::Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(values.data(), rows, values.size()/rows);
}

int main(int argc, char** argv){
  // HELP AND INPUT VALIDATIONS
  if(argc < 9 || !(argc % 2)) {print_help();}

  // MANDATORY PARAMETERS
  int method_id = -1;
  string train_set_path;
  string test_set_path;
  string classification_output_path;
  
  // OPTIONAL PARAMETERS
  int knn_neighbors = 10;
  int pca_components = 10;
  //int kfold_partitions = 5;
  
  // LOAD PARAMETERS IN ANY ORDER
  uint mandatory_parameters_amount = 0;

  for (size_t i = 0; i < (uint(argc)-1)/2; i++) {
    string parameter = argv[1+(2*i)];
    string argument = argv[2+(2*i)];
      
    if (parameter == "-m") {
      method_id = stoi(argument);
      ++mandatory_parameters_amount;
    } else if (parameter == "-i") {
      train_set_path = argument;
      ++mandatory_parameters_amount;
    } else if (parameter == "-q") {
      test_set_path = argument;
      ++mandatory_parameters_amount;
    } else if (parameter == "-o") {
      classification_output_path = argument;
      ++mandatory_parameters_amount;
    } else if (parameter == "-k") {
      knn_neighbors = stoi(argument);
    } else if (parameter == "-p") {
      pca_components = stoi(argument);
    // } else if (parameter == "-f") {
    //   kfold_partitions = stoi(argument);
    } else {
      cout << "Unexpected parameter: " << parameter << endl;
      exit(1);
    }
  }

  // MANDATORY PARAMETERS VALIDATION
  if (mandatory_parameters_amount != 4) {print_help();}

  // LOAD IMAGES
  cout << "Loading input files..." << endl;
  Matrix train_set = load_from<MatrixXd>(train_set_path);
  Matrix test_set = load_from<MatrixXd>(test_set_path);

  Vector train_labels = train_set.col(0);
  //Vector test_labels = test_set.col(0);
  Matrix new_train_set = train_set(Eigen::all, Eigen::seq(1, Eigen::last));
  //Matrix new_test_set = test_set(Eigen::all, Eigen::seq(1, Eigen::last));
  Matrix new_test_set = test_set;

  Matrix classifications;
  KNNClassifier knn_method = KNNClassifier(knn_neighbors);

  if (method_id == 0) {
      cout << "Start kNN with " << knn_neighbors << " kNN neighbors" << endl;
      knn_method.fit(new_train_set, train_labels);
      classifications = knn_method.predict(new_test_set);
  } else if (method_id == 1) {
      cout << "Start PCA+kNN with " << pca_components << " PCA components and " << knn_neighbors << " kNN neighbors" << endl;
      PCA pca_method = PCA(pca_components);
      pca_method.fit(new_train_set);
      new_train_set = pca_method.transform(new_train_set);
      new_test_set = pca_method.transform(new_test_set);

      knn_method.fit(new_train_set, train_labels);
      classifications = knn_method.predict(new_test_set);
  } else {
			cout << "Error! Invalid <method_id>. Possible values: {0 : kNN | 1 : PCA+kNN}" << endl;
			exit(1);
	}

  // WRITE CLASSIFICATIONS
	ofstream output_file (classification_output_path);
  output_file << "ImageId,Label" << endl;

	for (int i = 0; i < classifications.rows(); ++i) {
		output_file << i+1 << "," << classifications(i,0) << endl;
	}
	output_file.close();

	return 0;
}
