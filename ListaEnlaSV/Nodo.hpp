#pragma once
#define TAM 1024

struct Cliente {
    int socket_fd = -1;
    char mensaje[TAM] = {0};
    char usuario[50] = {0};
    char contraseña[50] = {0};
    bool autenticado = false;
    bool estado = false;
};

class Nodo {
public:
    Cliente dato;
    Nodo* siguiente;
    Nodo(Cliente valor) : dato(valor), siguiente(nullptr) {}
};