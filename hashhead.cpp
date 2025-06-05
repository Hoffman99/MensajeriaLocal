#include "Hashheader.hpp"

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
            return;
        }

        indice = (indice + 1) % CAPACIDAD;
        intentos++;
    }

    cout << "Tabla llena. No se pudo insertar " << clave << endl;
}

bool TablaHash::iniciarsesion(const string& usuario, const Clave& clave) {
    int indice = funcionHash(clave);
    int intentos = 0;

    while (intentos < CAPACIDAD) {
        if (tabla[indice].estado == OCUPADO && tabla[indice].usuario == usuario && tabla[indice].clave == clave) {
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



