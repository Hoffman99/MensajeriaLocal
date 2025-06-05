#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define PUERTO 25565
#define TAM 1024

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    sockaddr_in servidor;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    inet_pton(AF_INET, "127.0.0.1", &servidor.sin_addr); 

    if (connect(sock, (sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Error al conectar al servidor");
        return 1;
    }

    char mensaje[TAM];
    char respuesta[TAM];

    while (true) {
        ssize_t bytes = recv(sock, respuesta, TAM, 0);
        if (bytes > 0) {
            cout << "Servidor respondió: " << respuesta << endl;
        } else if (bytes == 0) {
            cout << "Servidor desconectado.\n";
            break;
        } else {
            perror("Error al recibir datos");

        }
        cout << "Escribe un mensaje: ";
        cin.getline(mensaje, TAM);

        send(sock, mensaje, strlen(mensaje), 0);

        memset(respuesta, 0, TAM);
    }

    close(sock);
    return 0;
}
