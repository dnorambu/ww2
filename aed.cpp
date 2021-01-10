#include "aed.hpp"
//Constructor de la clase AED
Aed::Aed(int id, int pos_original, int pos_actual, bool rep, bool nuevo){
    m_id = id;
    m_pos_original = pos_original;
    m_pos_actual = pos_actual;
    m_reposicionado = rep;
    m_nuevo = nuevo;
}