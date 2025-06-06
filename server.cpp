#include "server.hpp"

void registrarUsuarios(TablaHash& tabla);

// crea y configura el socket del servidor
int crearSocketServidor(sockaddr_in& direccion);

// aceptar a alguien que intenta conectase
void aceptarCliente(int socket_servidor, Cliente clientes[], TablaHash& tabla);

// recibir mensajes de cada cliente y reenviarlos a los demás (broadcast)
void manejarMensajes(Cliente clientes[]);

int main() {
    TablaHash tabla;
    registrarUsuarios(tabla); 

    Cliente clientes[MAX_CLIENTES];  

    sockaddr_in direccion;
    int socket_servidor = crearSocketServidor(direccion);
    if (socket_servidor < 0) return 1;

    cout << "Servidor escuchando en el puerto " << PUERTO << endl;

    // bucle principal, para escucha y respuesta
    while (true) {
        fd_set lectura_fds;
        FD_ZERO(&lectura_fds);

        // se añade el socket del servidor al conjunto de sockets (para nuevas conexiones)
        FD_SET(socket_servidor, &lectura_fds);
        int max_fd = socket_servidor;

        // se añaden todos los clientes conectados al conjunto de sockets (el compilado que yo les habia dicho)
        for (int i = 0; i < MAX_CLIENTES; i++) {
            if (clientes[i].socket_fd != -1) {
                FD_SET(clientes[i].socket_fd, &lectura_fds);
                if (clientes[i].socket_fd > max_fd)
                    max_fd = clientes[i].socket_fd;
            }
        }

        // timeout de espera (50ms)
        timeval timeout = {0, 50000};

        // select escucha todos los sockets a la vez sin bloquearse
        int actividad = select(max_fd + 1, &lectura_fds, NULL, NULL, &timeout);

        if (actividad < 0) {
            perror("Error en select");
            break;
        }

        // si hay actividad en el socket del servidor, es un nuevo cliente
        if (FD_ISSET(socket_servidor, &lectura_fds)) {
            aceptarCliente(socket_servidor, clientes, tabla);
        }

        // revisar si algún cliente envió mensaje
        manejarMensajes(clientes);
    }

    close(socket_servidor);  // se cierra el servidor al terminar
    return 0;
}
