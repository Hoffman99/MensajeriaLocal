#include "hashheader.hpp"
#include <iostream>
// Implementación de métodos de TablaHash

TablaHash::TablaHash() {
    for (int i = 0; i < CAPACIDAD; ++i) {
        tabla[i].estado = VACIO; // Usa '=' en vez de '=='
    }
}

int TablaHash::funcionHash(const Clave& clave) {
    int suma = 0;
    for (char c : clave) {
        suma += c;
    }
    return suma % CAPACIDAD;
}

void TablaHash::Registrar(const string& usuario, const string& clave) {
    int indice = funcionHash(clave);
    int intentos = 0;

    while (intentos < CAPACIDAD) {
        if (tabla[indice].estado == OCUPADO && tabla[indice].clave == clave && tabla[indice].usuario == usuario) {
            std::cout << "El usuario ya existe " << endl;
            return;
        }

        if (tabla[indice].estado != OCUPADO) {  
            tabla[indice].clave = clave; 
            tabla[indice].usuario = usuario;
            tabla[indice].estado = OCUPADO;
            tabla[indice].online = false;
            return;
        }

        indice = (indice + 1) % CAPACIDAD;
        intentos++;
    }

    cout << "Tabla llena. No se pudo insertar " << clave << endl;
}

int TablaHash::iniciarsesion(const string& usuario, const Clave& clave) {
    int indice = funcionHash(clave);
    int intentos = 0;

    while (intentos < CAPACIDAD) {
        if (tabla[indice].estado == OCUPADO && tabla[indice].usuario == usuario && tabla[indice].clave == clave) {
            if(tabla[indice].online==false){
                tabla[indice].online=true;
                return 0;
            }else {
                return 1;
            }
        }

        if (tabla[indice].estado == VACIO) {
            return 2;
        }

        indice = (indice + 1) % CAPACIDAD;
        intentos++;
    }
    return 2;
}