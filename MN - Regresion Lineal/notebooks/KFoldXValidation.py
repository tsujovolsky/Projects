import pandas as pd
from sklearn.neighbors import KNeighborsClassifier
from sklearn import linear_model
import numpy as np
import itertools 
import metnum
import math

def rmse(predicciones,originales):
    res = 0
    n = len(predicciones)
    for i in range(0,n):
        res += (predicciones[i] - originales[i])**2
    res = res / n
    return float(np.sqrt(res))

def rmsle(predicciones,originales):
    res = 0
    flag = 0
    n = len(predicciones)
    for i in range(0,n):
        if predicciones[i] < -1:
            res += 0
            flag = 1
        else:
            res += (np.log(predicciones[i] + 1) - np.log(originales[i] + 1))**2
    res = res / n
    if flag == 1:
        return -1
    return float(np.sqrt(res))

def r_squared(predicciones,originales):
    SSE = 0
    TSS = 0
    n = len(predicciones)
    y_promedio = 0
    for i in range(0,n):
        y_promedio += originales[i]
        SSE += (predicciones[i] - originales[i])**2
    y_promedio = y_promedio / n
    for i in range(0,n):
        TSS += (originales[i] - y_promedio)**2
    return float(1 - (SSE / TSS))
        
    


class KfoldXvalidation:
    """
    Realiza el metodo de K-Fold Cross Validation para:
    - df : la base de datos,
    - K  : numero de particiones
    - knn: numero de vecinos cercanos para determinar la clase,
    - pca: realizando PCA en (pca) dimensiones solamente si este parametro es mayor a 0.
    """
    def __init__(self, datos, objetivo, K, log):
        self.rmse_list = []
        self.rmsle_list = []
        self.r_squared_list = []
        
        datos_count = datos.shape[0]
        muestras_por_fold = int(datos_count//K)
        leftover = int(datos_count % K)
        
        for i in range(0, K):
            low_lim = i * muestras_por_fold
            if i == K-1:
                high_lim = low_lim + muestras_por_fold + leftover
            else:
                high_lim = low_lim + muestras_por_fold
            
            datos_train = np.concatenate((datos[:low_lim],datos[high_lim:]))
            objetivo_train = np.concatenate((objetivo[:low_lim],objetivo[high_lim:]))
            datos_test = datos[low_lim:high_lim]
            objetivo_test = objetivo[low_lim:high_lim]

            lr = linear_model.LinearRegression()
            lr.fit(datos_train, objetivo_train)
            prediction = lr.predict(datos_test)
            
            self.rmse_list.append(rmse(prediction,objetivo_test))
            if log == 1:
                self.rmsle_list.append(rmsle(prediction,objetivo_test))

            self.r_squared_list.append(r_squared(prediction,objetivo_test))


    def r_squared(self):
        return self.r_squared_list
    def rmse_por_fold(self):
        return self.rmse_list
    def rmsle_por_fold(self):
        return self.rmsle_list