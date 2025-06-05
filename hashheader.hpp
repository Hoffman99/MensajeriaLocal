#pragma once 
#include <iostream>
#include <string>
#define CAPACIDAD 10
using namespace std;

typedef string Clave;
typedef int Valor;
typedef string user;
enum Estado { 
    VACIO,
    OCUPADO
};

struct Celda {
    Clave clave;
    user usuario;
    Valor valor;
    Estado estado;
};

class TablaHash {
private:
    Celda tabla[CAPACIDAD];

    int funcionHash(const Clave& clave);

public:

    TablaHash() ;
    //registrar una nueva persona 
    void Registrar(const string& usuario, const string& clave) ;
    //iniciar sesion
    bool iniciarsesion(const string& usuario,const Clave& clave);
    //crear usuarios 
    void crearuser();
    
};
