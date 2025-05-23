#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int puerto = 25565;

int main() {
    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_servidor, (sockaddr*)&direccion, sizeof(direccion)) < 0) {
        perror("bind");
        return 1;
    }
    
    listen(socket_servidor, 1);
    cout << "Servidor escuchando en " << puerto << "\n";

    sockaddr_in cliente;
    socklen_t cliente_len = sizeof(cliente);
    int socket_cliente = accept(socket_servidor, (sockaddr*)&cliente, &cliente_len);
    if (socket_cliente >= 0) {
        cout << "host conectado\n";

        const char* mensaje = "holaaaaaaa";
        send(socket_cliente, mensaje, strlen(mensaje), 0);

        close(socket_cliente);
    }

    close(socket_servidor);
    return 0;
}
