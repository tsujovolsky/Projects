#pragma once
#include "types.h"

/*
Calcula el autovalor (y autovector correspondiente) de módulo máximo

Parámetros:
----------

A: const Matrix& A
    Matriz sobre la que queremos calcular el autovalor

iterations: unsigned (=5000 por defecto)
    Cantidad de iteraciones a correr

epsilon: double
    Tolerancia a residuo (opcional)

Devuelve:
--------

pair<double, Vector> donde el primer valor es el autovalor
y el segundo el autovector asociado
*/
std::pair<double, Vector>
    power_iteration(const Matrix& A, unsigned iterations=5000, double epsilon=1e-16);


/*
Calcula

Parámetros:
----------

X: const Matrix& mat
    Matriz sobre la que queremos calcular el autovalor

amount: unsigned (=5000 por defecto)
    Cantidad de autovectores/autovalores a calcular

iterations: unsigned (=5000 por defecto)
    Cantidad de iteraciones a correr

epsilon: double
    Tolerancia a residuo (opcional)
Devuelve:
--------

pair<Vector, Matrix> donde:
    - El primero elemento es un vector de los autovalores
    - El segundo elemento es una matriz cuyas columnas son los autovectores
      correspondientes
*/
std::pair<Eigen::VectorXd, Matrix>
    get_first_eigenvalues(const Matrix& X, unsigned amount, unsigned iterations=5000, double epsilon=1e-16);
