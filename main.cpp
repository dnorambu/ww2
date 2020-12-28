//main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
#include <ctime>
#include <set>
#include <vector>
#include "funciones.hpp"

using namespace std;

const int max_iteracion = 100; //Maximo numero de iteraciones de HC
const int aed_iniciales = 10;  //Maximo n° de AED en una solucion inicial

int main(int argc, char** argv){
    //Pedir ruta de archivo
    string ruta;
    cout<<"Ruta de la instancia de prueba:";
    cin>>ruta;

    //Leer el archivo
    string linea;
    ifstream archivo(ruta);

    //Obtener los parametros iniciales
    getline (archivo, linea);
    int eventos, cobertura;
    float presupuesto;
    stringstream ssin(linea);
    
    ssin >> eventos;
    ssin >> presupuesto;
    ssin >> cobertura;
    
    int cx [eventos];               //Coordenadas x de las ubicaciones
    int cy [eventos];               //Coordenadas y de las ubicaciones
    int aed_in_position[eventos];   //Guarda las posiciones donde hay un AED instalado (1 si, 0 no)
    int eventos_cubiertos[eventos];      
    
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
    srand((unsigned) time(NULL));
    set<int> sol_in;
    vector<int> vec;
    
    while (true)
    {
        //Crear solucion inicial
        //Tomar una posición cualquiera y agregarla a la solucion inicial (si)
        int num = rand()%eventos;
        sol_in.insert(num);
        int posx_anterior = cx[num];
        int posy_anterior = cy[num];
        
        while (sol_in.size()<aed_iniciales)
        {
            //Implementacion de la funcion miope (detalles en README)
            num = rand()%eventos;
            int posx_nueva = cx[num];
            int posy_nueva = cy[num];
            if(distancia(posx_anterior,posy_anterior,posx_nueva,posy_nueva)>= cobertura){
                sol_in.insert(num);
                posx_anterior = posx_nueva;
                posy_anterior = posy_nueva;
             eventos_cubiertos[num] += 1;
            }
        }

        break;
    }
    // printing set s1
    set<int, greater<int> >::iterator itr;
    cout << "\nThe set s1 is : \n";
    for (itr = sol_in.begin(); 
         itr != sol_in.end(); ++itr) 
    {
        cout << *itr << ",";
    }
    cout << eventos << endl;
    return 0;
}
