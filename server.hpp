#pragma once
#include "ListaEnlazada.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include "hashheader.hpp"
#include <fstream>

using namespace std;

#define PUERTO 25565
#define TAM 1024
#define MAX_CLIENTES 10

void escribir_logs(const string& mensaje);

void registrarUsuarios(TablaHash& tabla);

// crea y configura el socket del servidor
int crearSocketServidor(sockaddr_in& direccion);

// aceptar a alguien que intenta conectase
void aceptarCliente(int socket_servidor, Cliente clientes, TablaHash& tabla);

// recibir mensajes de cada cliente y reenviarlos a los demás (broadcast)
void manejarMensajes(Cliente clientes, TablaHash& tabla);
