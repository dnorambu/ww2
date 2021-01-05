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
#include "aed.hpp"

//Maximo numero de restarts del algoritmo
const int max_restart = 3;
//Maximo n° de AED en una solucion inicial 
unsigned int max_aed = 10;  
//Maximo numero de fallas al intentar mejorar una solucion
const int max_fail = 20;
//Minimo de vecinos para Greedy
const int min_vec = 35;
//Generador de id para los AED
int id_aed = 0;

using namespace std;

int main(int argc, char** argv){
    //SEED basada en tiempo actual
    srand((unsigned) time(NULL));   
    
    //Variables historicas
    //Mejor resultado de la funcion de evaluacion
    int optimo_h = 0;
    //Solucion que entregó el mejor resultado
    unordered_set<int> solucion_h;
    //Mapa que asocia posiciones con ID de AED
    map<int,int> movs_h;
    float presupuesto_h;

    //Variables locales
    //Resultado obtenido en la iteracion
    int optimo_l = 0;
    //Solucion modificada durante la iteracion
    unordered_set<int> solucion_l;
    //Mapa que asocia posiciones con ID de AED
    map<int,int> movs_l;

    //Variables nueva_posiliares
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
    int id;
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
    unordered_set<int> preinstaladas;
    //Guarda las posiciones donde hay un AED instalado 
    unordered_set<int> instaladas;
    //Guarda las posiciones candidatas para instalar un AED
    unordered_set<int> candidatas;   
    //ecpv -> Evento Cubierto Por Vecino (ecpv[i] += 1 por cada vecino i que lo cubra)
    //En base a este registro se calculará la función de evaluación
    vector<int> ecpv(eventos);
    //Arreglo que guarda los vecinos de cada posicion               
    unordered_set<int> vecindario[eventos];
    //Conjunto de AEDs existentes
    vector<Aed> aed_existentes;
    vector<Aed> aed_existentes_h;
    //Guardar la información del archivo en memoria
    int i = 0;
    //aed es 1 (true) o 0 (false)
    bool aed_instalado;

    while (getline (archivo,linea)){
        stringstream ssin(linea);
        ssin >> cx[i];
        ssin >> cy[i];
        ssin >> aed_instalado;
        if(aed_instalado){
            preinstaladas.insert(i);
        }else{
            candidatas.insert(i);
        }
        i++;
    }
    //Cerrar archivo
    archivo.close();
    
    buscar_vecinos(cx,cy,cobertura,eventos,vecindario);

    //Comienza el algoritmo

    while(restart < max_restart){
        //Asignar presupuesto inicial
        presupuesto_temp = presupuesto;
        //Crear solucion inicial
        instaladas = preinstaladas;
        solucion_inicial(candidatas,instaladas,vecindario,cobertura,max_aed,min_vec);
        //Crear los objetos AED
        for (int posicion: instaladas)
        {
            //Construccion del AED implicita en emplace_back
            aed_existentes.emplace_back(id_aed,posicion,false,false);
            //Guardar la posicion actual en el mapa local
            movs_l.insert({posicion,id_aed});
            id_aed++;    
        }
         
        //Cubrir a los vecinos del conjunto "instaladas" inicial
        for(int ubicacion: instaladas){
            agregar_cob(vecindario[ubicacion], ecpv);
        }
        //Evaluar la cobertura inicial
        optimo_l = cobertura_total(ecpv);
        cout<<"Optimo local: "<<optimo_l<<" Iteracion: "<<restart<<" Size ecpv: "<<ecpv.size()<<" Size aed_exist: "<<aed_existentes.size()<<endl;
        for(Aed wea: aed_existentes){cout<<wea.getId()<<",";}
        cout<<"\n";
        //Guardar el estado actual en copias
        solucion_l = instaladas;
        cand_copy = candidatas;
        ecpv_temp = ecpv;
        
        //Reparar la solucion inicial
        while ((presupuesto_temp >= costo_instalacion) && (fail < max_fail))
        {
            /*
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
                        presupuesto_temp -= costo_swap;
                        //El valor asociado a la llave ubic_aed es el id del AED que fue movido
                        id = movs_l[ubic_aed];
                        //Recordar el movimiento.
                        movs_l[ubic_cand] = id;
                        //Ahora ubic_aed ya no tiene un aed instalado, se borra del mapa
                        movs_l.erase(ubic_aed);
                        //Recordar que el id del AED es también su posición en el vector
                        aed_existentes[id].setPosActual(ubic_cand);
                        aed_existentes[id].setRep(true);
                        //Romper el for interno pues se logró una mejora 
                        break;
                    }else{
                        //Deshacer los cambios
                        swap_pos(ubic_cand,ubic_aed,solucion_l,cand_copy);
                        ecpv_temp = ecpv;
                        fail++;
                    } 
                }
            }
            */
            //Terminamos de hacer swaps, instalamos un nuevo AED si el presupuesto lo permite
            if(presupuesto_temp >= costo_instalacion){
                presupuesto_temp -= costo_instalacion;
                int nueva_pos = select_random(cand_copy);
                cand_copy.erase(nueva_pos);
                instaladas.insert(nueva_pos);
                agregar_cob(vecindario[nueva_pos],ecpv_temp);
                optimo_l=cobertura_total(ecpv_temp);
                aed_existentes.emplace_back(id_aed,nueva_pos,false,true);
                id_aed++;
            }else
            {
                fail++;
            }
        }
        //Termina de repararse la solucion inicial, comparar con la marca historica
        cout<<"ML: "<<optimo_l<<" MH: "<<optimo_h<<endl;
        if(optimo_l > optimo_h){
            optimo_h = optimo_l;
            solucion_h = solucion_l;
            movs_h = movs_l;
            presupuesto_h = presupuesto_temp;
            aed_existentes_h = aed_existentes;
        }
        //Fin del ciclo, resetear variables
        id_aed = 0;
        aed_existentes.clear();
        movs_l.clear();
        instaladas.clear();
        solucion_l.clear();
        cand_copy.clear();
        memset(&ecpv_temp[0], 0, ecpv_temp.size() * sizeof ecpv_temp[0]);
        memset(&ecpv[0], 0, ecpv.size() * sizeof ecpv[0]);
        restart++;
    }
    
    //Fin del algoritmo, entregar resultados
    //Cobertura maxima alcanzada
    cout<<"1. Maximo optimo encontrado: "<<optimo_h<<endl;
    cout<<"2. Porcentaje de cobertura: "<<(optimo_h*100)/eventos<<endl;
    //Printear las posiciones de los AED y si son nuevos o reposicionados
    cout<<"AED's instalados"<<endl;
    cout<<"ID \t Estado \t Pos. Original \t  Pos. Actual \t nuevo?"<<endl;
    for(int posicion: solucion_h){
        id = movs_h[posicion];
        Aed aed_temp = aed_existentes_h[id];
        int x_or = cx[aed_temp.getPosOrig()];
        int y_or = cy[aed_temp.getPosOrig()];
        int x_ac = cx[aed_temp.getPosActual()];
        int y_ac = cy[aed_temp.getPosActual()];
        if(aed_temp.getNuevo()){
            cout<<id<<"\t"<<" Nuevo \t ("<<x_or<<","<<y_or<<")  ("<<x_ac<<","<<y_ac<<") "<<aed_temp.getNuevo()<<endl;
        }else{
            cout<<id<<"\t"<<" Reposicionado \t ("<<x_or<<","<<y_or<<")  ("<<x_ac<<","<<y_ac<<") "<<aed_temp.getNuevo()<<endl;
        }
    }
    cout<<"Presupuesto sobrante: "<<presupuesto_h<<endl;
    return 0;
}

