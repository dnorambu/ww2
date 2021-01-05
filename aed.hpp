#ifndef AED
#define AED

class Aed{  
    private:
        int m_id;
        int m_pos_original;
        int m_pos_actual;
        bool m_reposicionado;
        bool m_nuevo;
    public:
        Aed(int id, int pos_original, bool rep, bool nuevo);
        //getters
        int getId(){return m_id;}
        int getPosOrig(){return m_pos_original;}
        int getPosActual(){return m_pos_actual;}
        bool getRep(){return m_reposicionado;}
        bool getNuevo(){return m_nuevo;}
        //setters
        void setPosActual(int posActual){m_pos_actual = posActual;}
        void setRep(bool rep){m_reposicionado = rep;}
        void setNuevo(bool nuevo){m_nuevo = nuevo;}
};

Aed& foo();
#endif