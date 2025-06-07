#pragma once 
#include <string>
#define CAPACIDAD 10
using namespace std;

typedef string Clave;
typedef int Valor;
typedef string user;
typedef bool Online;

enum Estado { 
    VACIO,
    OCUPADO
};

struct Celda {
    Clave clave;
    user usuario;
    Valor valor;
    Estado estado;
    Online online;
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
    int iniciarsesion(const string& usuario,const Clave& clave);
    //Desconectar usuarios al salir
    void Desconectar(const string& usuario, const Clave& clave);
    
};
