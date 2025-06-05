#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

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
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds); 

        int max_fd = sock > STDIN_FILENO ? sock : STDIN_FILENO;

        timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        int actividad = select(max_fd + 1, &readfds, NULL, NULL, &tv);

        if (actividad < 0) {
            perror("Error en select");
            break;
        } else if (actividad == 0) {
            cout << "\nTiempo para escribir expirado. Revisando mensajes...\n";
            continue;
        }

        if (FD_ISSET(sock, &readfds)) {
            ssize_t bytes = recv(sock, respuesta, TAM - 1, 0);
            if (bytes > 0) {
                respuesta[bytes] = '\0';
                cout << "\nMensaje recibido: " << respuesta <<"\n";
            } else if (bytes == 0) {
                cout << "Servidor desconectado.\n";
                break;
            } else {
                perror("Error al recibir datos");
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(mensaje, TAM, stdin) != NULL) {
                size_t len = strlen(mensaje);
                if (len > 0 && mensaje[len-1] == '\n') {
                    mensaje[len-1] = '\0';
                }
                send(sock, mensaje, strlen(mensaje), 0);
            }
        }
    }

    close(sock);
    return 0;
}
