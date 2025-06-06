#include "ListaEnlazada.hpp"
#include "Nodo.hpp"
#include <iostream>
#include <cstring>
using namespace std;

Nodo* cabeza;  
Nodo* cola;

// Verifica si la lista está vacía
bool Lista::estaVacia()  {
    return cabeza == nullptr;
}

// Insertar elemento al inicio de la lista
void Lista::insertarAlInicio(Cliente valor) {
    Nodo* nuevoNodo = new Nodo(valor);
    nuevoNodo->siguiente = cabeza;
    cabeza = nuevoNodo;
    cantidad++;
    // Si la lista estaba vacía, actualizar cola
    if (cola == nullptr) {
        cola = cabeza;
    }
}

// Insertar elemento al final de la lista
void Lista::insertarAlFinal(Cliente valor) {
    Nodo* nuevoNodo = new Nodo(valor);
    cantidad++;
    nuevoNodo->dato.estado=true;
    if (estaVacia()) {
        cabeza = cola = nuevoNodo;
    } else {
        cola->siguiente = nuevoNodo;
        cola = nuevoNodo;
    }
}

// Mostrar todos los elementos de la lista
void Lista::mostrar(){
    Nodo* actual = cabeza;
    cout << "Elementos de la lista: ";
    while (actual != nullptr) {
        cout << actual->dato.usuario << " ";
        actual = actual->siguiente;
    }
    cout << endl;
}

// Función para buscar un elemento en la lista
// Retorna un puntero al nodo que contiene el elemento o nullptr si no se encuentra
Nodo* Lista::buscarElemento(Cliente valor) {
    Nodo* actual = cabeza;

    while (actual != nullptr) {
        if (actual->dato.socket_fd == valor.socket_fd &&
            strcmp(actual->dato.mensaje, valor.mensaje) == 0 &&
            strcmp(actual->dato.usuario, valor.usuario) == 0 &&
            strcmp(actual->dato.contraseña, valor.contraseña) == 0 &&
            actual->dato.autenticado == valor.autenticado) {
            return actual;  // Elemento encontrado
        }
        actual = actual->siguiente;
    }

    return nullptr;  // Elemento no encontrado
}

// Método para eliminar un nodo por su valor
bool Lista::eliminarNodo(Cliente valor) {
    if (estaVacia()) {
        return false;
    }
    
    Nodo* actual = cabeza;
    Nodo* anterior = nullptr;
    
    // Buscar el nodo a eliminar
    while (actual != nullptr &&
           (actual->dato.socket_fd != valor.socket_fd ||
            strcmp(actual->dato.mensaje, valor.mensaje) != 0 ||
            strcmp(actual->dato.usuario, valor.usuario) != 0 ||
            strcmp(actual->dato.contraseña, valor.contraseña) != 0 ||
            actual->dato.autenticado != valor.autenticado)) {
        anterior = actual;
        actual = actual->siguiente;
    }
    
    // Si no se encontró el valor
    if (actual == nullptr) {
        return false;
    }
    
    // Caso 1: Eliminar el primer nodo
    if (actual == cabeza) {
        cabeza = cabeza->siguiente;
        // Si era el único nodo, actualizar cola
        if (cabeza == nullptr) {
            cola = nullptr;
        }
    }
    // Caso 2: Eliminar nodo intermedio o final
    else {
        anterior->siguiente = actual->siguiente;
        // Si era el último nodo, actualizar cola
        if (actual == cola) {
            cola = anterior;
        }
    }
    delete actual;
    cantidad--;
    return true;
}

Nodo* Lista::obtenerCabeza(){
    return cabeza;
}

int Lista::ObtenerCantidad(){
    return cantidad;
}

