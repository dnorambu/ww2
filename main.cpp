//main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <array>
#include "funciones.hpp"
#include "aed.hpp"

//Maximo numero de restarts del algoritmo
const int max_restart = 3;
//Maximo n° de AED en una solucion inicial 
unsigned int max_aed = 6;  
//Minimo de vecinos para Greedy
const int min_vec = 15;
//Generador de id para los AED
int id_aed = 0;
const float costo_instalacion = 1;
const float costo_swap = 0.2;



int main(int argc, char** argv){
    //SEED basada en tiempo actual
    srand((unsigned) time(NULL));   
    
    //Variables historicas
    //Mejor resultado de la funcion de evaluacion
    int optimo_h = 0;
    //Solucion que entregó el mejor resultado
    std::unordered_set<int> solucion_h;
    float presupuesto_h;

    //Variables locales
    //Resultado obtenido en la iteracion
    int optimo_l = 0;
    //Solucion modificada durante la iteracion
    std::unordered_set<int> solucion_l;

    //Variables auxiliares
    //Resultado para verificar mejoras
    int optimo_temp = 0;
    //Contador de restart realizados
    int restart = 0;
    //Iteradores de conjuntos
    std::unordered_set<int>::iterator itr;
    std::unordered_set<int>::iterator vec;
    //Presupuesto de cada iteracion
    float presupuesto_temp;
    //indicador de intentos fallidos al intentar encontrar una mejora cualquiera (escapar de optimo local)
    unsigned int fail;
    int dummy;
    //switch para indicar mejoras en la reparacion
    bool mejora;
    //Pedir ruta de archivo
    std::string ruta;
    std::cout<<"Ruta (relativa) de la instancia de prueba:";
    std::cin>>ruta;

    //Leer el archivo
    std::string linea;
    std::ifstream archivo(ruta);

    //Obtener los parametros iniciales
    getline (archivo, linea);
    //Numero total de ubicaciones
    int eventos;
    //Radio de cobertura de un AED
    int cobertura;
    //Presupuesto total de la instancia
    float presupuesto;
    std::stringstream ssin(linea);
    ssin >> eventos;
    ssin >> presupuesto;
    ssin >> cobertura;

    //Coordenadas x de las ubicaciones
    int cx [eventos];               
    //Coordenadas y de las ubicaciones
    int cy [eventos];               
    //Guarda las posiciones donde hay un AED instalado previamente
    std::unordered_set<int> preinstaladas;
    //Guarda las posiciones donde hay un AED instalado 
    std::unordered_set<int> instaladas;
    //Guarda las posiciones candidatas para instalar un AED
    std::unordered_set<int> candidatas;   
    //ecpv -> Evento Cubierto Por V cino (ecpv[i] += 1 por cada v    cino i que lo cubra)
    //En base a este registro se calculará la función de evaluación
    std::vector<int> ecpv(eventos);
    //Arreglo que guarda los v  cinos de cada posicion               
    std::unordered_set<int> vecindario[eventos];
    //Conjunto de AEDs existentes
    std::vector<Aed> aed_existentes;
    std::vector<Aed> aed_existentes_h;
    //Guardar la información del archivo en memoria
    int i = 0;
    //aed es 1 (true) o 0 (false)
    bool aed_instalado;

    while (getline (archivo,linea)){
        std::stringstream ssin(linea);
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
        //Esta linea es para respetar posibles aed preinstalados (segun lo indicado en el .txt)
        //en la creacion de nuevas soluciones iniciales. Sin este paso, el algoritmo no
        //consideraría tal información y las instancias no tendrían diferencia alguna al
        //ejecutar el codigo
        instaladas = preinstaladas;
        //Crear solucion inicial
        solucion_inicial(candidatas,instaladas,vecindario,cobertura,max_aed,min_vec);
        //Asignar presupuesto inicial
        presupuesto_temp = presupuesto;
        //Crear los objetos AED iniciales
        for(int posicion: instaladas){
            //Construccion del AED implicita en emplace_back
            aed_existentes.emplace_back(id_aed,posicion,posicion,false,false);
            id_aed++;    
        }
        //Cubrir a los vecinos del conjunto "instaladas" inicial
        for(int ubicacion: instaladas){
            agregar_cob(vecindario[ubicacion], ecpv);
        }
        //Evaluar la cobertura inicial
        optimo_l = cobertura_total(ecpv);
        fail = 0;
        dummy = 0;
        std::cout<<"\nAED Existentes! Size: "<<aed_existentes.size()<<std::endl;
        for(Aed aed_aux: aed_existentes){
            std::cout<<aed_aux.getId()<<" "<<aed_aux.getPosOrig()<<" "<<aed_aux.getPosActual()<<std::endl;
        }
        //Reparar la solucion inicial 
        while ((fail <= aed_existentes.size()) && (presupuesto_temp >= costo_swap))
        {   
            int indice_aed = 0;
            //Intentar un swap por cada ubicacion de la solucion inicial
            for(Aed aed: aed_existentes){
                //No iterar si el anterior swap agotó el presupuesto
                if(presupuesto_temp < costo_swap){
                    break;
                }
                int u_con_aed = aed.getPosActual();
                unsigned int intentos = 0;
                mejora = false;
                while((!mejora) && (intentos < candidatas.size())){
                    //Tomar una ubicacion de las candidatas para hacer un swap
                    for(int u_sin_aed: candidatas){
                        intentos++;
                        //Recordar:
                        // Si el swap mejora la solución, u_con_aed saldrá de la solución,
                        // siendo reemplazada por u_sin_aed
                        //Debe modificarse ecpv primero para evaluar la nueva configuracion 
                        //Quitarle cobertura a los vecinos de aed
                        eliminar_cob(vecindario[u_con_aed],ecpv);
                        //Dar cobertura a los vecinos de u_sin_aed
                        agregar_cob(vecindario[u_sin_aed],ecpv);
                        optimo_temp = cobertura_total(ecpv);
                        if(optimo_temp > optimo_l){
                            swap_pos(u_con_aed,u_sin_aed,instaladas,candidatas);
                            presupuesto_temp -= costo_swap;
                            std::cout<<dummy<<" Swaping: sale "<<u_con_aed<<" entra:"<<u_sin_aed<<" pt: "<<presupuesto_temp<<"\n";
                            std::cout<<"AED POS ANT: "<<aed.getPosActual()<<" "<<aed.getPosOrig()<<"\n";
                            if(aed.getNuevo()){
                                Aed aed_temp(aed.getId(),aed.getPosOrig(),u_sin_aed,true,true);
                                aed_existentes[indice_aed] = aed_temp;
                            }else{
                                Aed aed_temp(aed.getId(),aed.getPosOrig(),u_sin_aed,true,false);
                                aed_existentes[indice_aed] = aed_temp;
                            }
                            std::cout<<"AED POS DES: "<<aed_existentes[indice_aed].getPosActual()<<" "<<aed_existentes[indice_aed].getPosOrig()<<std::endl;
                            optimo_l = optimo_temp;
                            dummy++;
                            mejora = true;
                            break;
                        }else{
                            //Marcha atrás ya que no se encontró una mejora en este intento
                            eliminar_cob(vecindario[u_sin_aed],ecpv);
                            agregar_cob(vecindario[u_con_aed],ecpv);
                        }
                    }
                    if(intentos == candidatas.size()){
                        //Fallo pues no hubo un swap que mejore la cobertura proporcionada por el aed en la posicion actual
                        fail++;
                    }
                    indice_aed++;
                }
            }
            std::cout<<"Fin de swaps"<<std::endl;
            std::cout<<"\nInstaladas! Size: "<<instaladas.size()<<" -> ";
            for (int k : instaladas){
                std::cout<<k<<",";
            }
            std::cout<<"\nAED Existentes! Size: "<<aed_existentes.size()<<std::endl;
            for(Aed aed_aux: aed_existentes){
                std::cout<<aed_aux.getId()<<" "<<aed_aux.getPosOrig()<<" "<<aed_aux.getPosActual()<<std::endl;
            }
            std::cout<<"\nPT: "<<presupuesto_temp<<" FAIL: "<<fail<<"\n"<<std::endl;
            //Terminamos de hacer swaps, instalamos un nuevo AED si el presupuesto lo permite
            if(presupuesto_temp >= costo_instalacion){
                presupuesto_temp -= costo_instalacion;
                int nueva_pos = select_random(candidatas);
                candidatas.erase(nueva_pos);
                instaladas.insert(nueva_pos);
                agregar_cob(vecindario[nueva_pos],ecpv);
                optimo_l=cobertura_total(ecpv);
                aed_existentes.emplace_back(id_aed,nueva_pos,nueva_pos,false,true);
                id_aed++;
                fail = 0;
            }
        }
        // Termina de repararse la solucion inicial, comparar con la marca historica
        std::cout<<"ML: "<<optimo_l<<" MH: "<<optimo_h<<std::endl;
        if(optimo_l > optimo_h){
            optimo_h = optimo_l;
            solucion_h = instaladas;
            presupuesto_h = presupuesto_temp;
            aed_existentes_h = aed_existentes;
        }
        //Reiniciar variables en cada restart
        id_aed = 0;
        restart++;
        aed_existentes.clear();
        memset(&ecpv[0], 0, ecpv.size() * sizeof ecpv[0]);
    }
    //Fin del algoritmo, entregar resultados
    //Cobertura maxima alcanzada
    
    std::cout<<"1. Maximo optimo encontrado: "<<optimo_h<<std::endl;
    std::cout<<"2. Porcentaje de cobertura: "<<(optimo_h*100)/eventos<<std::endl;
    //Printear las posiciones de los AED y si son nuevos o reposicionados
    std::cout<<"AED's instalados"<<std::endl;
    std::cout<<"ID \t Estado \t Pos. Original \t  Pos. Actual \t nuevo?\t(x,y)or \t (x,y)ac"<<std::endl;
    for(Aed aed_temp: aed_existentes_h){
        int x_or = cx[aed_temp.getPosOrig()];
        int y_or = cy[aed_temp.getPosOrig()];
        int x_ac = cx[aed_temp.getPosActual()];
        int y_ac = cy[aed_temp.getPosActual()];
        if(aed_temp.getNuevo()){
            std::cout<<aed_temp.getId()<<"\t"<<" Nuevo \t \t (----,----) \t  ("<<x_ac<<","<<y_ac<<") "<<aed_temp.getNuevo()<<"\t"<<aed_temp.getPosOrig()<<"\t"<<aed_temp.getPosActual()<<std::endl;
        }else{
            std::cout<<aed_temp.getId()<<"\t"<<" Reposicionado \t ("<<x_or<<","<<y_or<<")  ("<<x_ac<<","<<y_ac<<") "<<aed_temp.getNuevo()<<"\t"<<aed_temp.getPosOrig()<<"\t"<<aed_temp.getPosActual()<<std::endl;
        }
    }
    std::cout<<"Presupuesto sobrante: "<<presupuesto_h<<std::endl;
    return 0;
}
