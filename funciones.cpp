#include "funciones.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_set>
using namespace std;

// distancia retorna la distancia euclideana entre 2 puntos
float distancia(int x1, int y1, int x2, int y2){
    return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}

// solucion_inicial crea una solución inicial representada por un conjunto de ubicaciones
// donde se instala un número fijo de AED's (max_aed)
void solucion_inicial(int cx[],int cy[], int aed_in_p[],int cobertura,int eventos, unsigned int max_aed, unordered_set<int> & sol_in){
    //Tomar una posición cualquiera y agregarla a la solucion inicial (sol_in)
    int num = rand()%eventos;
    sol_in.insert(num);
    int posx_anterior = cx[num];
    int posy_anterior = cy[num];
    
    //La solucion inicial siempre instalará hasta "max_aed" desfibriladores
    while (sol_in.size()<max_aed)
    {
        //Implementacion de la funcion miope (detalles en README)
        num = rand()%eventos;
        int posx_nueva = cx[num];
        int posy_nueva = cy[num];
        if(distancia(posx_anterior,posy_anterior,posx_nueva,posy_nueva)>= cobertura){
            sol_in.insert(num);
            posx_anterior = posx_nueva;
            posy_anterior = posy_nueva;
            aed_in_p[num] = 1;
        }
    }
}

// buscar_vecinos tiene el propósito de encontrar las posiciones alrededor
// de un punto A (donde se ha instalado un AED) que estén dentro del radio.
// El parámetro ecpv representa a un array que mantiene un contador por cada
// ubicación que está cubierta por algún vecino; ecpv[i] es 0 si ningún vecino
// posee un AED instalado que pueda cubrir a la posición "i"; en cambio, ecpv[i]
// tendrá valor n > 0 si hay "n" vecinos que tengan un AED instalado que cubra
// a la posición "i". 
// "vecindario" es un array que guarda el conjunto de vecinos de cada posición "i"
void buscar_vecinos(int cx[], int cy[], int id, int cobertura, int rango, int ecpv[], unordered_set<int> vecindario[]){
    unordered_set<int> vecinos;
    // Punto A = (x,y)
    int x = cx[id];
    int y = cy[id];
    
    // los vecinos los encontraremos usando a (cx[i], cy[i])
    for (int i = 0; i < rango; i++)
    {
        //Dato: una posición A se considera vecina de si misma
        if(distancia(x,y,cx[i],cy[i])<= cobertura)
        {  
            //Encontramos un vecino, se agrega al conjunto de vecinos
            vecinos.insert(i);
            //Ese vecino ahora está cubierto
            ecpv[i]+=1;
        } 
    }
    vecindario[id] = vecinos;        
}
// eliminar_cobertura recibe un conjunto de vecinos de una posición y
// les quita la cobertura, modificando el array ecpv. Esta función se aplica 
// cuando un AED instalado previamente en una posición A se mueve a 
// una nueva posición B, por lo que los vecinos de A ya no estarán cubiertos.  
void eliminar_cobertura(unordered_set<int> & vecinos, int ecpv[]){
    unordered_set<int>::iterator itr;
    for (itr = vecinos.begin(); itr != vecinos.end(); ++itr) 
    {
        ecpv[*itr] -= 1;
    }
}

// cobertura_total lee el array "ecpv" y muestra por pantalla
// cuantas posiciones están cubiertas de acuerdo a la solución actual
// De esta manera, podemos evaluar si la solución actual mejora la marca previa
int cobertura_total(int ecpv[], int rango){
    int resultado = 0;
    for(int i = 0; i < rango; i++){
        if(ecpv[i] > 0){
            resultado += 1;
        }
    }
    return resultado;
}
//Template para buscar un elemento aleatorio de un conjunto
template<typename S>
auto select_random(const S &s, size_t n) {
  auto it = std::begin(s);
  // 'advance' the iterator n times
  std::advance(it,n);
  return it;
}
// swap toma el array de aed instalados, hace un intercambio de posiciones
// entre una posición i que posee un AED instalado y una posición j que
// no tiene AED. Luego, modifica el conjunto solución para representar el
// cambio hecho.
void swap_pos(int aed_in_p[],unordered_set<int>&solucion, int posicion_entrante, int posicion_saliente, int eventos, int *pca,int *psa){
    //Hacer el cambio en aed_in_p[]
    aed_in_p[posicion_entrante] = 1;
    aed_in_p[posicion_saliente] = 0;
    //Modificar el conjunto solucion
    solucion.erase(posicion_saliente);
    solucion.insert(posicion_entrante);
    //Asignar las posiciones cambiadas a los punteros para mantener un historial
    *pca = posicion_entrante; //pca -> Posicion Con AED
    *psa = posicion_saliente; //psa -> Posicion Sin AED
}
//si_mejora es una funcion que revisa si una nueva solucion generada a partir de un swap mejora
//la marca anterior historica
bool si_mejora(int cx[], int cy[], int id, int cobertura, int rango, int ecpv[], unordered_set<int> vecindario[],
    unordered_set<int> & solucion, int eventos, int *pca,int *psa, int *optimo_historico,int *resultado_actual,
    unordered_set<int> & sol_final){
    bool mejora;
    
}

void leer_sol(unordered_set<int> & solucion){
    unordered_set<int>::iterator itr;
    cout << "\nThe set s1 is : \n";
    for (itr = solucion.begin(); 
         itr != solucion.end(); ++itr) 
    {
        cout << *itr << ",";
    }
    cout<<endl;
}
/*
Tener un arreglo de "Eventos cubiertas POR VECINOS" (ecpv)
Tener un arreglo de tuplas? Hacer una estructura que tenga la información

¿Necesito los vecinos siempre? No, solo los necesito para actualizar ecpv
luego de eso se vuelve información inutil, a menos que tuviera que por 
alguna razón volver a calcular los vecinos, pero puede que eso no ocurra muy seguido.
*/