#ifndef FUNCIONES
#define FUNCIONES
#include <unordered_set>
//Constantes del algoritmo
const int costo_instalación = 1;
const float costo_swap = 0.2;
//Funciones del algoritmo
void buscar_vecinos(int cx[], int cy[],int id, int radio, int rango, int ecpv[], std::unordered_set<int> vecindario[]);
void solucion_inicial(int cx[],int cy[], int eventos_cubiertos[],int radio,int eventos, unsigned int max_aed, std::unordered_set<int> & sol_in);
float distancia(int,int,int,int);
int cobertura_total(int ecpv[], int rango);
void leer_sol(std::unordered_set<int> & solucion);
void swap_pos(int aed_in_p[],unordered_set<int>&solucion, int posicion_entrante, int posicion_saliente, int eventos, int *pca,int *psa);
void eliminar_cobertura(std::unordered_set<int> & vecinos, int ecpv[]);

#endif