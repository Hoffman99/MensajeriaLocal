#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

#define PUERTO 25565
#define TAM 256
#define MAX_CLIENTES 3

struct Cliente {
    int socket_fd = -1;
    char mensaje[TAM] = {0};
    char usuario[50] = {0};
    char contraseña[50] = {0};
    bool autenticado = false;
};

int main() {
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
       
        sockaddr_in cliente_dir;
        socklen_t cliente_len = sizeof(cliente_dir);
        int socket_cliente = accept(socket_servidor, (sockaddr*)&cliente_dir, &cliente_len);

        if (socket_cliente >= 0) {
         
            bool agregado = false;
            for (int i = 0; i < MAX_CLIENTES; i++) {
                if (clientes[i].socket_fd == -1) {
                    clientes[i].socket_fd = socket_cliente;
                    cout << "Cliente conectado en la posición " << i << endl;
                    agregado = true;
                    break;
                }
            }
            if (!agregado) {
                cout << "Demasiados clientes conectados. Cerrando nueva conexión.\n";
                close(socket_cliente);
            }
        }

    
        for (int i = 0; i < MAX_CLIENTES; i++) {
            if (clientes[i].socket_fd != -1) {
                char buffer[TAM];
                ssize_t bytes = recv(clientes[i].socket_fd, buffer, TAM, MSG_PEEK);
                if (bytes > 0) {
                    bytes = recv(clientes[i].socket_fd, clientes[i].mensaje, TAM, 0);
                    cout << "Cliente " << i << " dice: " << clientes[i].mensaje << endl;
                    send(clientes[i].socket_fd, clientes[i].mensaje, strlen(clientes[i].mensaje), 0);
                    memset(clientes[i].mensaje, 0, TAM);
                } else if (bytes == 0) {
             
                    cout << "Cliente " << i << " desconectado.\n";
                    close(clientes[i].socket_fd);
                    clientes[i].socket_fd = -1;
                }
            }
        }

        usleep(50000);
    }

    close(socket_servidor);
    return 0;
}
