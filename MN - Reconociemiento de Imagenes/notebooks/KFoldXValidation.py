import pandas as pd
from sklearn.neighbors import KNeighborsClassifier
import numpy as np
import itertools 
import metnum
from sklearn.metrics import accuracy_score, precision_score, recall_score, confusion_matrix


class KfoldXvalidation:
    """
    Realiza el metodo de K-Fold Cross Validation para:
    - df : la base de datos,
    - K  : numero de particiones
    - knn: numero de vecinos cercanos para determinar la clase,
    - pca: realizando PCA en (pca) dimensiones solamente si este parametro es mayor a 0.
    """
    def __init__(self, df, K, knn, pca):
        self.accuracies = []
        self.precisions = []
        self.recalls = []
        self.confusion_matrices = []
        
        images = df[df.columns[1:]].values
        labels = df["label"].values.reshape(-1, 1)
        images_count = images.shape[0]
        images_per_fold = int(images_count//K)
        leftover = int(images_count % K)
        
        for i in range(0, K):
            low_lim = i * images_per_fold
            if i == K-1:
                high_lim = low_lim + images_per_fold + leftover
            else:
                high_lim = low_lim + images_per_fold
            
            images_train = np.concatenate((images[:low_lim],images[high_lim:]))
            labels_train = np.concatenate((labels[:low_lim],labels[high_lim:]))
            images_test = images[low_lim:high_lim]
            labels_test = labels[low_lim:high_lim]

            if pca > 0:
                pca_train = metnum.PCA(pca)
                pca_train.fit(images_train)
                images_train = pca_train.transform(images_train)
                images_test = pca_train.transform(images_test)

            clf = metnum.KNNClassifier(knn)
            clf.fit(images_train, labels_train)
            
            prediction = clf.predict(images_test)
            
            accuracy = accuracy_score(labels_test, prediction)
            precision = precision_score(labels_test, prediction, average = "macro")
            recall = float(recall_score(labels_test, prediction, average = "macro"))
            confused_matrix = confusion_matrix(labels_test, prediction)

            self.accuracies.append(accuracy)
            self.precisions.append(precision)
            self.recalls.append(recall)
            self.confusion_matrices.append(confused_matrix)
            
        self.average_accuracy = np.mean(self.accuracies)
        self.average_precision = np.mean(self.precisions)
        self.average_recall = np.mean(self.recalls)
        #self.average_confusion_matrix = ?
            #for i in range(0, K):
                #self.average_confusion_matrix = ?
            #self.average_confusion_matrix = ?
            
                
        
    def accuracy(self):
        return self.accuracies
    def precision(self):
        return self.precisions
    def recall(self):
        return self.recalls
    def confusion_matrix(self):
        return self.confusion_matrices
    def f_score(self,beta):
        return (1 + beta**2) * (self.average_precision * self.average_recall) / (self.average_recall + (beta**2 * self.average_precision))
