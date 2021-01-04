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
#include <array>
#include "funciones.hpp"

//Maximo numero de restarts del algoritmo
const int max_restart = 3;
//Maximo n° de AED en una solucion inicial 
unsigned int max_aed = 10;  
//Maximo numero de fallas al intentar mejorar una solucion
const int max_fail = 20;
//Minimo de vecinos para Greedy
const int min_vec = 35;

using namespace std;

int main(int argc, char** argv){
    //SEED basada en tiempo actual
    srand((unsigned) time(NULL));   
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
    //Coordenadas x de las ubicaciones
    int cx [eventos];               
    //Coordenadas y de las ubicaciones
    int cy [eventos];               
    //Guarda las posiciones donde hay un AED instalado previamente
    unordered_set<int> instaladas;
    //Guarda las posiciones candidatas para instalar un AED
    unordered_set<int> candidatas;   
    //ecpv -> Evento Cubierto Por Vecino (ecpv[i] += 1 por cada vecino i que lo cubra)
    //En base a este registro se calculará la función de evaluación
    vector<int> ecpv(eventos);
    //Arreglo que guarda los vecinos de cada posicion               
    unordered_set<int> vecindario[eventos];

    //Guardar la información del archivo en memoria
    int i = 0;
    //aed es 1 (true) o 0 (false)
    bool aed;
    while (getline (archivo,linea)){
        stringstream ssin(linea);
        ssin >> cx[i];
        ssin >> cy[i];
        ssin >> aed;
        if(aed){
            instaladas.insert(i);
        }else{
            candidatas.insert(i);
        }
        i++;
    }
    //Cerrar archivo
    archivo.close();
    
    buscar_vecinos(cx,cy,cobertura,eventos,vecindario);

    //Variables historicas
    //Mejor resultado de la funcion de evaluacion
    int optimo_h = 0;
    //Solucion que entregó el mejor resultado
    unordered_set<int> solucion_h;
    //Mapa que recuerda si hubo movimientos
    map<int,bool> movs_h;

    //Variables locales
    //Resultado obtenido en la iteracion
    int optimo_l = 0;
    //Solucion modificada durante la iteracion
    unordered_set<int> solucion_l;
    //Mapa que recuerda movimientos
    map<int,bool> movs_l;

    //Variables auxiliares
    //Copia de las posiciones candidatas
    unordered_set<int> cand_copy;
    //Resultado para verificar mejoras
    int optimo_temp = 0;
    //Contador de restart realizados
    int restart = 0;
    //Copia del vector ecpv
    vector<int> ecpv_temp;
    //Iteradores de conjuntos
    unordered_set<int>::iterator itr;
    unordered_set<int>::iterator vec;
    //Presupuesto de cada iteracion
    float presupuesto_temp;
    //indicador de intentos fallidos al intentar encontrar una mejora cualquiera (escapar de optimo local)
    int fail;
    //Comienza el algoritmo
    
    while(restart < max_restart){
        //Asignar presupuesto inicial
        presupuesto_temp = presupuesto;
        //Crear solucion inicial
        solucion_inicial(candidatas,instaladas,vecindario,cobertura,max_aed,min_vec);
        //Cubrir a los vecinos del conjunto "instaladas" inicial
        for(int ubicacion: instaladas){
            agregar_cob(vecindario[ubicacion], ecpv);
        }
        //Evaluar la cobertura inicial
        optimo_l = cobertura_total(ecpv);

        //Guardar el estado actual en copias
        solucion_l = instaladas;
        cand_copy = candidatas;
        ecpv_temp = ecpv;
        //Reparar la solucion inicial
        while ((presupuesto_temp > costo_swap) || (fail < max_fail))
        {
            //Intentar un swap por cada ubicacion de la solucion inicial
            for(int ubic_aed: instaladas){
                for(int ubic_cand: cand_copy){
                    swap_pos(ubic_aed,ubic_cand,solucion_l,cand_copy);
                    //Debe modificarse ecpv_temp primero para evaluar la nueva configuracion 
                    //Quitarle cobertura a los vecinos de ubic_aed
                    eliminar_cob(vecindario[ubic_aed],ecpv_temp);
                    //Dar cobertura a los vecinos de ubic_cand
                    agregar_cob(vecindario[ubic_cand],ecpv_temp);
                    optimo_temp = cobertura_total(ecpv_temp);
                    //Si existe una mejora, actualizamos las variables
                    if(optimo_temp>optimo_l){
                        optimo_l = optimo_temp;
                        //Recordar el movimiento. Esto solo ocurre 1 vez, cuando se mueve el
                        //AED a una posición diferente de la que tenía en la solución inicial
                        //Si la ubicacion no está en movs_l es porque no se había movido de su
                        //posición original.
                        if(movs_l.find(ubic_aed) == movs_l.end()){
                            bool aux = true;
                            movs_l.insert({ubic_aed,true});
                        }
                    } 
                }
            }
        }
        
        //Fin del ciclo, resetear variables
        optimo_l = 0;
        movs_l.clear();
        solucion_l.clear();
        ecpv_temp.clear();
        ecpv.clear();
        restart++;
    }
    /*
    //Conjunto que representa a una solucion inicial
    unordered_set<int> sol_in;                
    //Conjunto solucion final optimo localmente
    unordered_set<int> sol_final;             
    //Conjunto que guardará la información de la solución inicial de la iteración donde se encontró el óptimo          
    unordered_set<int> sol_in_copia; 
    //Conjunto que se modificará en las iteraciones, copia del conjunto de la iteracion anterior
    unordered_set<int> sol_temp; 
    //vector para agregar nuevos AED instalados
    vector<int> nuevos; 
    
    int intentos_swap;
    //PCA (Posicion Con Aed) guarda a la posición que recibe un AED tras un swap
    int pca;
    //PSA (Posicion Sin Aed) guarda a la posición que pierde un AED tras un swap
    int psa;
    int resultado_actual;
    int posicion_saliente, posicion_entrante;
    bool stop = false;
    while ()
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
                        presupuesto_temp -= costo_swap;
                        //guardar la posición original del AED
                        if(pos_originales.find(posicion_entrante) == pos_originales.end()){
                            //Como la llave no está, significa que no se ha guardado la ubicacion original
                            pos_originales.insert({posicion_entrante,make_tuple(cx[posicion_entrante],cy[posicion_entrante])});
                        }
                        //Rompemos el ciclo for para avanzar y hacer swap con la siguiente posición
                        break;
                    }else{
                        //No hubo mejora, por lo que debe deshacerse el movimiento 
                        //Llamar a swap_pos con los argumentos *itr y posicion_entrante cambiados 
                        swap_pos(aed_in_position,sol_temp,*itr,posicion_entrante,eventos,&pca,&psa);
                        eliminar_cobertura(vecindario[pca],ecpv);
                        //Volver a cubrir los vecinos anteriores
                        for (vec = vecindario[psa].begin() ; vec != vecindario[psa].end();vec++)
                        {
                            ecpv[*vec] += 1;
                        }
                    }
                }//Terminan los swaps, intentar instalar un nuevo AED
                while(!stop){
                    posicion_entrante = rand()%eventos;
                    if(aed_in_position[posicion_entrante] == 0){
                        stop = true;
                    }
                }
                stop = false;
                
                if(!vecindario[pca].empty()){
                    for (vec = vecindario[pca].begin() ; vec != vecindario[pca].end();vec++)
                    {
                        ecpv[*vec] += 1;
                    }
                }else{//De lo contrario buscar sus vecinos 
                    buscar_vecinos(cx,cy,pca,cobertura,eventos,ecpv,vecindario);
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
    */
    return 0;
}

