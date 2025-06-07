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
    // crea el socket del cliente (IPv4, TCP)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    // configura la dirección del servidor
    sockaddr_in servidor;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO); // convierte el puerto al formato de red
    inet_pton(AF_INET, "127.0.0.1", &servidor.sin_addr); // IP local

    // intenta conectar al servidor
    if (connect(sock, (sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Error al conectar al servidor");
        return 1;
    }

    char mensaje[TAM];
    char respuesta[TAM];

    // --- Autenticación inicial ---

    // recibe "Usuario: "
    ssize_t bytes = recv(sock, respuesta, TAM - 1, 0);
    if (bytes > 0) {
        respuesta[bytes] = '\0';
        cout << respuesta;

        // escribe y envía el usuario
        cin.getline(mensaje, TAM);
        send(sock, mensaje, strlen(mensaje), 0);
    }

    // recibe "password: "
    bytes = recv(sock, respuesta, TAM - 1, 0);
    if (bytes > 0) {
        respuesta[bytes] = '\0';
        cout << respuesta;

        // escribe y envía la contraseña
        cin.getline(mensaje, TAM);
        send(sock, mensaje, strlen(mensaje), 0);
    }

    // recibe mensaje de bienvenida o error
    bytes = recv(sock, respuesta, TAM - 1, 0);
    if (bytes > 0) {
        respuesta[bytes] = '\0';
        cout << respuesta << endl;

        // si es incorrecto, cierra y sale
        if (strstr(respuesta, "incorrectos") != nullptr) {
            close(sock);
            return 0;
        }
    }

    while (true) {

        // preparar el set para select
        fd_set readfds;
        FD_ZERO(&readfds);              // limpia el set
        FD_SET(sock, &readfds);         // agrega el socket (por si el server manda algo)
        FD_SET(STDIN_FILENO, &readfds); // agrega la entrada del teclado

        // define el más grande entre socket y teclado
        int max_fd = sock > STDIN_FILENO ? sock : STDIN_FILENO;

        // timeout para select (10 segundos)
        timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        // select revisa si hay actividad en teclado o socket sin bloquear
        int actividad = select(max_fd + 1, &readfds, NULL, NULL, &tv);

        if (actividad < 0) {
            perror("Error en select");
            break;
        }

        // si el servidor mandó algo
        if (FD_ISSET(sock, &readfds)) {
            ssize_t bytes = recv(sock, respuesta, TAM - 1, 0);
            if (bytes > 0) {
                respuesta[bytes] = '\0';
                cout << "Mensaje de "<< respuesta << endl;
            } else if (bytes == 0) {
                cout << "Servidor desconectado.\n";
                break;
            } else {
                perror("Error al recibir datos");
                break;
            }
        }

        // si el usuario escribió algo
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(mensaje, TAM, stdin) != NULL) {
                // limpia el salto de línea al final
                size_t len = strlen(mensaje);
                if (len > 0 && mensaje[len-1] == '\n') {
                    mensaje[len-1] = '\0';
                }
                // manda el mensaje al servidor
                send(sock, mensaje, strlen(mensaje), 0);
            }
        }
    }

    // se cierra el socket cuando todo termina
    close(sock);
    return 0;
}
