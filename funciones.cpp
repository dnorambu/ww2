#include "funciones.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_set>
using namespace std;

// distancia retorna la distancia euclideana entre 2 puntos
float distancia(int x1, int y1, int x2, int y2){
    return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}
//Para seleccionar elementos aleatorios de un conjunto
int select_random(unordered_set<int> &a){
    int n = rand()%a.size();
    unordered_set<int>::iterator itr;
    itr = a.begin();
    std::advance(itr,n);
    return *itr;
}

// solucion_inicial toma posiciones candidatas y las mueve al conjunto
// de posiciones instaladas
void solucion_inicial(unordered_set<int> &candidatas, unordered_set<int> &instaladas,
                    unordered_set<int> vecindario[],int cobertura, unsigned int max_aed, 
                    unsigned int min_vec)
    {
    int candidato;
    //La solucion inicial siempre instalará hasta "max_aed" desfibriladores
    while (instaladas.size()<max_aed)
    {
        //Implementacion de la funcion miope (detalles en README)
        candidato = select_random(candidatas);
        if(vecindario[candidato].size()>min_vec){
            instaladas.insert(candidato);
        }
    }
}

//Para cada ubicacion i, encontrar las ubicaciones j que estén
//dentro de su radio de cobertura. (incluye cuando i = j)
void buscar_vecinos(int cx[], int cy[], int cobertura, int eventos, unordered_set<int> vecindario[]){
    for(int i = 0; i < eventos; i++){
        for(int j = 0; j < eventos; j++){
            if(distancia(cx[i],cy[i],cx[j],cy[j]) <= cobertura){
                vecindario[i].insert(j);
            }
        }
    }
}

// cobertura_total lee el vector "ecpv" y cuenta
// cuantas posiciones están cubiertas de acuerdo a la solución actual
// De esta manera, podemos evaluar si la solución actual mejora la marca previa
int cobertura_total(vector<int> &ecpv){
    int resultado = 0;
    for(int posicion: ecpv){
        if(posicion > 0){
            resultado += 1;
        }
    }
    return resultado;
}

// swap_pos toma 2 posiciones (p1 y p2) e intercambia el conjunto al que pertenecen
// p1 pertenece a K, p2 pertenece a Q (originalmente)
// K : conjunto de posiciones con AED ya instalado
// Q : conjunto de posiciones sin AED (candidatas)
void swap_pos(int p1, int p2, unordered_set<int> &K,unordered_set<int> &Q){
    K.erase(p1);
    K.insert(p2);
    Q.erase(p2);
    Q.insert(p1);
}
//agregar_cob suma +1 a cada vecino de la posicion i en el vector ecpv.
//"i" es una posicion donde se instaló un nuevo AED, o se movió un AED
//existente hacia esta posición.
void agregar_cob(unordered_set<int> &vecinos, vector<int> &ecpv){
    for(int vecino: vecinos){
        ecpv[vecino] += 1;
    }
}
//eliminar_cob resta -1 a cada vecino de la posicion i en el vector ecpv.
//"i" es una posición que perdió el AED que tenía instalado
void eliminar_cob(unordered_set<int> &vecinos, vector<int> &ecpv){
    for(int vecino: vecinos){
        ecpv[vecino] -= 1;
    }
}
