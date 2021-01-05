#include "aed.hpp"
//Constructor de la clase AED
Aed::Aed(int id, int pos_original, bool rep, bool nuevo){
    m_id = id;
    m_pos_original = pos_original;
    m_pos_actual = pos_original;
    m_reposicionado = rep;
    m_nuevo = nuevo;
}