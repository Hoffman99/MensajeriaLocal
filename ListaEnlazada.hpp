#pragma once
#include "Nodo.hpp"

class Lista {
private:
    Nodo* cabeza;  // Puntero al primer nodo
    Nodo* cola;    // Puntero al último nodo (para inserción eficiente)
    int cantidad;

public:
    // Constructor: crea lista vacía
    Lista() : cabeza(nullptr), cola(nullptr), cantidad(0) {}
    
    // Verifica si la lista está vacía
    bool estaVacia();
    
    // Insertar elemento al inicio de la lista
    void insertarAlInicio(Cliente valor);
    
    // Insertar elemento al final de la lista
    void insertarAlFinal(Cliente valor);
    
    // Mostrar todos los elementos de la lista
    void mostrar();
    
    // Función para buscar un elemento en la lista
    // Retorna un puntero al nodo que contiene el elemento o nullptr si no se encuentra
    Nodo* buscarElemento(Cliente valor);
    
    // Método para eliminar un nodo por su valor
    bool eliminarNodo(Cliente valor);
    
    Nodo* obtenerCabeza();

    int ObtenerCantidad();

    // Destructor: libera toda la memoria
    ~Lista(){
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            Nodo* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        cabeza = cola = nullptr;
    }
};