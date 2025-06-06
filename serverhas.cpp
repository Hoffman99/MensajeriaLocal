#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include "hashheader.hpp"

using namespace std;

#define PUERTO 25565
#define TAM 1024
#define MAX_CLIENTES 10


struct Cliente {
    int socket_fd = -1;
    char mensaje[TAM] = {0};
    char user[50] = {0};
    char password[50] = {0};
    bool autenticado = false;
};




int main() {
   //inicializar la tabla hash
    TablaHash tabla;
    tabla.Registrar("usuario1", "1234");
    tabla.Registrar("usuario2", "abcd");
    tabla.Registrar("usuario3", "qwerty");
    tabla.Registrar("usuario4", "asdfgh");
    tabla.Registrar("usuario5", "zxcvbn");
    tabla.Registrar("usuario6", "qazwsx");
    tabla.Registrar("usuario7", "123456");
    tabla.Registrar("usuario8", "password");
    tabla.Registrar("usuario9", "letmein");
    tabla.Registrar("usuario10", "iloveyou");
    //inicializar los clientes
    Cliente clientes[MAX_CLIENTES];  
    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);
    direccion.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_servidor, (sockaddr*)&direccion, sizeof(direccion)) < 0) {
        perror("Error al hacer bind");
        return 1;
    }

    if (listen(socket_servidor, 5) < 0) {
        perror("Error en listen");
        return 1;
    }

    cout << "Servidor escuchando en el puerto " << PUERTO << endl;

    while (true) {
        fd_set lectura_fds;
        FD_ZERO(&lectura_fds);
        FD_SET(socket_servidor, &lectura_fds);
        int max_fd = socket_servidor;

        for (int i = 0; i < MAX_CLIENTES; i++) {
            if (clientes[i].socket_fd != -1) {
                FD_SET(clientes[i].socket_fd, &lectura_fds);
                if (clientes[i].socket_fd > max_fd)
                    max_fd = clientes[i].socket_fd;
            }
        }

        timeval timeout = {0, 50000}; 
        int actividad = select(max_fd + 1, &lectura_fds, NULL, NULL, &timeout);

        if (actividad < 0) {
            perror("Error en select");
            break;
        }

        if (FD_ISSET(socket_servidor, &lectura_fds)) {
            sockaddr_in cliente_dir;
            socklen_t cliente_len = sizeof(cliente_dir);
            int socket_cliente = accept(socket_servidor, (sockaddr*)&cliente_dir, &cliente_len);
            if (socket_cliente >= 0) {
                bool agregado = false;
                for (int i = 0; i < MAX_CLIENTES; i++) {
                    if (clientes[i].socket_fd == -1) {
                        clientes[i].socket_fd = socket_cliente;
                        cout << "Cliente conectado en la posición " << i << endl;
                        cout << "Ingrese su usuario: ";
                        cin >> clientes[i].user;
                        cout << "Ingrese su contraseña: ";
                        cin >> clientes[i].password;
                        if (tabla.iniciarsesion(clientes[i].user, clientes[i].password)) {
                            const char* bienvenida = "¡Bienvenido al servidor!";
                            send(clientes[i].socket_fd, bienvenida, strlen(bienvenida), 0);
                            agregado = true;
                            break;
                        }
                        else {
                            const char* error = "Usuario o contraseña incorrectos. Conexión cerrada.";
                            send(clientes[i].socket_fd, error, strlen(error), 0);
                            close(socket_cliente);
                            cout << "Cliente " << i << " no autenticado. Conexión cerrada.\n";
                        }
                    }
                }
                if (!agregado) {
                    cout << "Demasiados clientes conectados. Cerrando nueva conexión.\n";
                    close(socket_cliente);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTES; i++) {
            if (clientes[i].socket_fd != -1 && FD_ISSET(clientes[i].socket_fd, &lectura_fds)) {
                ssize_t bytes = recv(clientes[i].socket_fd, clientes[i].mensaje, TAM, 0);
                if (bytes > 0) {
                    cout << "Cliente " << i << " dice: " << clientes[i].mensaje << endl;
                    for (int j = 0; j < MAX_CLIENTES; j++)
                    {
                        if (clientes[j].socket_fd!=clientes[i].socket_fd && clientes[j].socket_fd!= -1)
                        {
                            send(clientes[j].socket_fd, clientes[i].mensaje, strlen(clientes[i].mensaje), 0);
                        }
                        
                    }
                    memset(clientes[i].mensaje, 0, TAM);
                } else if (bytes == 0 || bytes < 0) {
                    cout << "Cliente " << i << " desconectado.\n";
                    close(clientes[i].socket_fd);
                    clientes[i].socket_fd = -1;
                }
            }
        }
    }

    close(socket_servidor);
    return 0;
}
