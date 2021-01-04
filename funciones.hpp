#ifndef FUNCIONES
#define FUNCIONES
//Constantes del algoritmo
#include <unordered_set>
#include <vector>
const int costo_instalacion = 1;
const float costo_swap = 0.2;
//Funciones del algoritmo
void buscar_vecinos(int cx[], int cy[], int cobertura, int eventos, std::unordered_set<int> vecindario[]);
float distancia(int,int,int,int);
int cobertura_total(vector<int> &ecpv);
void agregar_cob(std::unordered_set<int> &vecinos, vector<int> &ecpv);
void eliminar_cob(unordered_set<int> &vecinos, vector<int> &ecpv);
void swap_pos(int p1, int p2, unordered_set<int> &K,unordered_set<int> &Q);
void solucion_inicial(std::unordered_set<int> &candidatas, 
                    std::unordered_set<int> &instaladas,
                    std::unordered_set<int> vecindario[],
                    int cobertura, unsigned int max_aed, 
                    unsigned int min_vec);
#endif