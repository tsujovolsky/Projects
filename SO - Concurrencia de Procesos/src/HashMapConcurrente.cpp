#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int hashindex = hashIndex(clave);
    bool agregue = false;
    buckets_mutex[hashindex].lock();
    for(auto &p : *tabla[hashindex]){
        if (p.first == clave) {
            p.second++;
            agregue = true;
            break;
        }
    }
    if (not(agregue)) tabla[hashindex]->insertar(hashMapPair(clave, 1));
    buckets_mutex[hashindex].unlock();

}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> claves;
    for (auto & i : tabla) {
        for(auto &p : *i){
            claves.push_back(p.first);
        }
    }
    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    unsigned int res = 0;
    for(auto &p : *tabla[hashIndex(clave)]) {
            if (p.first == clave) return p.second;
    }
    return res;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (auto &p : buckets_mutex){ //sacamos una foto del momento en el que llammamos la funcion
        p.lock();
    }

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
        buckets_mutex[index].unlock(); //liberamos el bucket que ya analizamos
    }

    return *max;
}

void HashMapConcurrente::thread_funct(ListaAtomica<hashMapPair>* l, std::atomic<int>* next_bucket){
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    while (1) {
        auto index = (unsigned int) next_bucket->fetch_add(1);
        if (index > 25) break;

        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
        buckets_mutex[index].unlock(); //liberamos el bucket que ya analizamos
    }

    l->insertar(*max);
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (auto &p : buckets_mutex){ //sacamos una foto del momento en el que llammamos la funcion
        p.lock();
    }

    ListaAtomica<hashMapPair> maximos_locales;

    std::thread threads[cant_threads];

    std::atomic<int> next_bucket{0};

    //creamos los threads
    for (unsigned int i = 0; i < cant_threads; ++i) {
        threads[i] = std::thread(&HashMapConcurrente::thread_funct, this, &maximos_locales, &next_bucket);
    }

    for (unsigned int i = 0; i < cant_threads; ++i) {
        threads[i].join();
    }

    for (auto &p : maximos_locales){
        if (p.second > max->second) {
            max->first = p.first;
            max->second = p.second;
        }
    }

    //recorremos maximos locales y tomamos el mayor
    return *max;
}

#endif
