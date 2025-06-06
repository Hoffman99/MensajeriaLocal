#include "server.hpp"

void registrarUsuarios(TablaHash& tabla){
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
}

// crea y configura el socket del servidor
int crearSocketServidor(sockaddr_in& direccion){
    // se crea el socket
    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Error al crear el socket");
        return -1;
    }

    // se llena la estructura con los datos del servidor la ip y su puerto
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);
    direccion.sin_addr.s_addr = INADDR_ANY;

    // se bindea el socket con la dirección
    if (bind(socket_servidor, (sockaddr*)&direccion, sizeof(direccion)) < 0) {
        perror("Error al hacer bind");
        return -1;
    }

    // se pone el socket en modo escucha
    if (listen(socket_servidor, 5) < 0) {
        perror("Error en listen");
        return -1;
    }

    return socket_servidor;
}

// aceptar a alguien que intenta conectase
void aceptarCliente(int socket_servidor, Cliente clientes[], TablaHash& tabla){
    sockaddr_in cliente_dir;
    socklen_t cliente_len = sizeof(cliente_dir);
    int socket_cliente = accept(socket_servidor, (sockaddr*)&cliente_dir, &cliente_len);

    if (socket_cliente >= 0) {
        bool agregado = false;
        ssize_t bytes;

        // buscar un espacio vacío para guardar al cliente
        for (int i = 0; i < MAX_CLIENTES; i++) {
            if (clientes[i].socket_fd == -1) {
                clientes[i].socket_fd = socket_cliente;
                cout << "Cliente conectado en la posición " << i << endl;

                // se pide el nombre de usuario
                const char* usu = "Usuario: ";
                send(clientes[i].socket_fd, usu, strlen(usu), 0);  // se envía el texto
                bytes = recv(clientes[i].socket_fd, clientes[i].user, TAM, 0);  // se recibe el nombre

                // se pide la contraseña
                const char* contr = "password ";
                send(clientes[i].socket_fd, contr, strlen(contr), 0);  // se envía
                bytes = recv(clientes[i].socket_fd, clientes[i].password, TAM, 0);  // se recibe

                // se revisa si el usuario existe y la contraseña es correcta
                if (tabla.iniciarsesion(clientes[i].user, clientes[i].password)) {
                    const char* bienvenida = "¡Bienvenido al servidor!";
                    send(clientes[i].socket_fd, bienvenida, strlen(bienvenida), 0);  // mensaje de bienvenida
                    agregado = true;
                    break;
                } else {
                    const char* error = "Usuario o contraseña incorrectos. Conexión cerrada.";
                    send(clientes[i].socket_fd, error, strlen(error), 0);
                    close(clientes[i].socket_fd);  // cerrar la conexión si falla el inicio de sesion
                    clientes[i].socket_fd = -1;
                    cout << "Cliente " << i << " no autenticado. Conexión cerrada.\n";
                    agregado = true;
                    break;
                }
            }
        }

        // no se acepta más si ya esta lleno
        if (!agregado) {
            cout << "Demasiados clientes conectados. Cerrando nueva conexión.\n";
            close(socket_cliente);
        }
    }
}

// recibir mensajes de cada cliente y reenviarlos a los demás (broadcast)
void manejarMensajes(Cliente clientes[]){
    for (int i = 0; i < MAX_CLIENTES; i++) {
        if (clientes[i].socket_fd != -1) {
            // se prepara el set de lectura para este cliente
            fd_set lectura_fds;
            FD_ZERO(&lectura_fds);
            FD_SET(clientes[i].socket_fd, &lectura_fds);
            timeval timeout = {0, 0}; // sin espera

            // revisa si un socket tiene datos disponibles para leer
            if (select(clientes[i].socket_fd + 1, &lectura_fds, NULL, NULL, &timeout) > 0) {
                // si hay datos, se reciben
                ssize_t bytes = recv(clientes[i].socket_fd, clientes[i].mensaje, TAM, 0);
                if (bytes > 0) {
                    // mensaje recibido
                    cout << "Cliente " << i << " dice: " << clientes[i].mensaje << endl;

                    // reenviar a todos los demás clientes. este es el broadcast
                    for (int j = 0; j < MAX_CLIENTES; j++) {
                        if (clientes[j].socket_fd != clientes[i].socket_fd && clientes[j].socket_fd != -1) {
                            send(clientes[j].socket_fd, clientes[i].mensaje, strlen(clientes[i].mensaje), 0);
                        }
                    }

                    // limpiar el buffer del cliente
                    memset(clientes[i].mensaje, 0, TAM);
                } else {
                    // si no hay bytes, el cliente se desconectó
                    cout << "Cliente " << i << " desconectado.\n";
                    close(clientes[i].socket_fd);
                    clientes[i].socket_fd = -1;
                }
            }
        }
    }
}

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
