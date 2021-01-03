//main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <map>
#include "funciones.hpp"

using namespace std;

//Maximo numero de restarts del algoritmo
const int max_restart = 3;
//Maximo n° de AED en una solucion inicial 
unsigned int aed_iniciales = 10;  
//Maximo numero de fallas al intentar mejorar una solucion
const int max_fail = 20;

int main(int argc, char** argv){
    //Pedir ruta de archivo
    string ruta;
    std::cout<<"Ruta de la instancia de prueba:";
    cin>>ruta;

    //Leer el archivo
    string linea;
    ifstream archivo(ruta);

    //Obtener los parametros iniciales
    getline (archivo, linea);
    //Numero total de ubicaciones
    int eventos;
    //Radio de cobertura de un AED
    int cobertura;
    //Presupuesto total de la instancia
    float presupuesto;
    stringstream ssin(linea);
    
    ssin >> eventos;
    ssin >> presupuesto;
    ssin >> cobertura;
    
    int cx [eventos];               //Coordenadas x de las ubicaciones
    int cy [eventos];               //Coordenadas y de las ubicaciones
    int aed_in_position[eventos];   //Guarda las posiciones donde hay un AED instalado (1 si, 0 no)
    int ecpv[eventos];              //ecpv -> Evento Cubierto Por Vecino
    unordered_set<int> vecindario[eventos];
    
    int i = 0;
    while (getline (archivo,linea)){
        //Guardar la información del archivo en memoria
        stringstream ssin(linea);
        ssin >> cx[i];
        ssin >> cy[i];
        ssin >> aed_in_position[i];
        i++;
    }
    //Cerrar archivo
    archivo.close();
    
    //Entramos al ciclo
    srand((unsigned) time(NULL));   //SEED basada en tiempo actual
    //Conjunto que representa a una solucion inicial
    unordered_set<int> sol_in;                
    //Conjunto solucion final optimo localmente
    unordered_set<int> sol_final;             
    //Conjunto que guardará la información de la solución inicial de la iteración donde se encontró el óptimo          
    unordered_set<int> sol_in_copia; 
    //Conjunto que se modificará en las iteraciones, copia del conjunto de la iteracion anterior
    unordered_set<int> sol_temp; 

    unordered_set<int>::iterator itr;
    int restart = 0;
    int optimo_historico;
    int optimo_local;
    float presupuesto_temp;
    //vector para agregar nuevos AED instalados
    vector<int> nuevos; 
    //mapa que mantiene el historial de las posiciones originales de la solucion inicial
    map<int,tuple<int,int>> pos_originales;
    //indicador de intentos fallidos al intentar encontrar una mejora cualquiera (escapar de optimo local)
    int fail;
    int intentos_swap;
    //PCA (Posicion Con Aed) guarda a la posición que recibe un AED tras un swap
    int pca;
    //PSA (Posicion Sin Aed) guarda a la posición que pierde un AED tras un swap
    int psa;
    int resultado_actual;
    int posicion_saliente, posicion_entrante;
    bool stop = false;
    while (restart < max_restart)
    {
        //Crear solucion inicial
        solucion_inicial(cx,cy,aed_in_position,cobertura,eventos,aed_iniciales, sol_in);
        //Encontrar los vecinos de las posiciones con AED en la solucion encontrada
        for (itr = sol_in.begin(); itr != sol_in.end(); ++itr) 
        {
            buscar_vecinos(cx,cy,*itr,cobertura,eventos,ecpv,vecindario);
        }
        //Tenemos el primer resultado de nuestra funcion de evaluacion para futura comparacion
        optimo_local = cobertura_total(ecpv,eventos);
        presupuesto_temp = presupuesto;
        sol_temp = sol_in;
        //Aplicar el movimiento swap o instalar nuevos AED hasta agotar el presupuesto
        //o quedarse atascado 
        while ((presupuesto_temp >= 0.2) && (fail < max_fail))
        {
            //Hacer un swap
            //Tomo una ubicacion cualquiera de la solucion inicial, no de la temporal.
            //De esta manera, el algoritmo intentará hacer 5 swaps por cada posición original
            //de la solucion (5 swaps = 1 nuevo aed, en términos de costo)
            for(itr = sol_in.begin(); itr != sol_in.end(); itr++){
                //La posición que se retira de la solución temporal es *itr
                //La posición que entra a la solución temporal es aleatoria y se elige a través
                //del array ecpv
                for(intentos_swap = 0; intentos_swap < 5; intentos_swap++){
                    while(!stop){
                    posicion_entrante = rand()%eventos;
                    if(aed_in_position[posicion_entrante] == 0){
                        stop = true;
                        }
                    }
                    //Para evitar que en la próxima iteración no entre al while
                    stop = false;
                    swap_pos(aed_in_position,sol_temp,posicion_entrante,*itr,eventos,&pca,&psa);
                    //Quitar cobertura a los vecinos de psa 
                    eliminar_cobertura(vecindario[psa],ecpv);    
                    //Cubrir a los vecinos de pca
                    //Si ya tenemos el vecindario de pca, modificamos ecpv directamente
                    unordered_set<int>::iterator vec;
                    if(!vecindario[pca].empty()){
                        for (vec = vecindario[pca].begin() ; vec != vecindario[pca].end();vec++)
                        {
                            ecpv[*vec] += 1;
                        }
                    }else{//De lo contrario buscar sus vecinos 
                        buscar_vecinos(cx,cy,pca,cobertura,eventos,ecpv,vecindario);
                    }
                    //Evaluar la solución temporal
                    if(optimo_local < cobertura_total(ecpv,eventos)){
                        //Encontramos una mejora
                        cout<<"Mejora! Antes: "<<optimo_local<< "Despues: "<<resultado_actual<<endl;
                        optimo_local = resultado_actual;
                        presupuesto_temp -= 0.2;
                        //guardar la posición original del AED

                        //Rompemos el ciclo for para avanzar y hacer swap con la siguiente posición
                        break;
                    }else{
                        //No hubo mejora, por lo que debe deshacerse el movimiento 
                        //Llamar a swap_pos con los argumentos *itr y posicion_entrante cambiados 
                        swap_pos(aed_in_position,sol_temp,*itr,posicion_entrante,eventos,&pca,&psa);
                        eliminar_cobertura(vecindario[pca],ecpv);
                        //Volver a cubrir los vecinos
                        for (vec = vecindario[psa].begin() ; vec != vecindario[psa].end();vec++)
                        {
                            ecpv[*vec] += 1;
                        }
                    }
                }
            }
            // for(itr = sol_temp.begin(); itr != sol_temp.end(); itr++){
            //     for(int j = 0; j < eventos-1;j++){
            //         if(aed_in_position[j] == 0){
            //             swap_pos(aed_in_position,sol_temp,j,*itr,eventos,&pca,&psa);
            //             
            //             eliminar_cobertura(vecindario[psa],ecpv);
                        
            //              
            //             
            //             else
            //             {
            //                 fail++;
            //                 //Deshacer el swap
            //             }
            //         }                    
            //     }    
            // }
        }
        
        //Reset del arreglo de eventos cubiertos
        memset(ecpv,0,sizeof(ecpv));
        //Reset del conjunto solución sol_in
        sol_in.clear();
        restart++;
    }
    
    //Mostrar resultados 
    std::cout<<"Total de eventos: "<<eventos<<endl;
    // printing set s1
    return 0;
}

