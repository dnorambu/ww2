#ifndef FUNCIONES
#define FUNCIONES
#include <unordered_set>
#include <vector>
//Constantes del algoritmo
const int costo_instalacion = 1;
const float costo_swap = 0.2;
//Funciones del algoritmo
void buscar_vecinos(int cx[], int cy[], int cobertura, int eventos, std::unordered_set<int> vecindario[]);
float distancia(int x1,int y1,int x2,int y2);
int select_random(std::unordered_set<int> &a);
int cobertura_total(std::vector<int> &ecpv);
void agregar_cob(std::unordered_set<int> &vecinos, std::vector<int> &ecpv);
void eliminar_cob(std::unordered_set<int> &vecinos, std::vector<int> &ecpv);
void swap_pos(int p1, int p2, std::unordered_set<int> &K, std::unordered_set<int> &Q);
void solucion_inicial(std::unordered_set<int> &candidatas, 
                    std::unordered_set<int> &instaladas,
                    std::unordered_set<int> vecindario[],
                    int cobertura, unsigned int max_aed, 
                    unsigned int min_vec);
#endif