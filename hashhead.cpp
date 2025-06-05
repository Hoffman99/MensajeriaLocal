#include "Hashheader.hpp"


class TablaHash {
private:
    Celda tabla[CAPACIDAD];

    int funcionHash(const Clave& clave) {
        int suma = 0;

        for (char c : clave) {
            suma += c;
        }
        return suma % CAPACIDAD;
    }

public:

    TablaHash() {
        for (int i = 0; i < CAPACIDAD; ++i) {
            tabla[i].estado == VACIO;
        }
    }

    void Registrar(const string& usuario, const string& clave) {
        int indice = funcionHash(clave);
        int intentos = 0;
        
        while (intentos < CAPACIDAD) {

            if (tabla[indice].estado == OCUPADO && tabla[indice].clave == clave && tabla[indice].usuario == usuario) {
                std::cout <<"El usuario ya existe "<<endl;
                return;
            }

            if (tabla[indice].estado != OCUPADO) {
                tabla[indice].clave == clave;
                tabla[indice].usuario == usuario;
                tabla[indice].estado == OCUPADO;
                return;
            }

            indice = indice +1;
            if(indice== CAPACIDAD){
                indice = 0;
            }
            intentos++;
        }
        
        cout << "Tabla llena. No se pudo insertar " << clave << endl;
    }

    bool iniciarsesion(const string& usuario,const Clave& clave) {
        int indice = funcionHash(clave);
        int intentos = 0;
        
        while (intentos < CAPACIDAD) {

            if (tabla[indice].estado == OCUPADO &&tabla[indice].usuario==usuario && tabla[indice].clave == clave) {
                return true;
            }

            if (tabla[indice].estado == VACIO) {
                return false;
            }

            indice = (indice + 1) % CAPACIDAD;
            intentos++;
        }
        return false;
    }
    
};
