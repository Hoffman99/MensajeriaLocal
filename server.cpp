#include "server.hpp"
#include <cstdio>

void escribir_logs(const string& mensaje, bool svclose=false) {
    ofstream logs("logs.txt", ios::app);
    time_t hora = time(0);
    char* fecha = ctime(&hora);
    if (fecha && fecha[24] == '\n') fecha[24] = '\0';
    if (logs.is_open()&&svclose==false) {
        logs << "[" << fecha << "] " << mensaje << endl;
        logs.close();
    }else {
        logs.close(); 
        string nuevo_nombre = string(fecha) + "_logs.txt";
        if (rename("logs.txt", nuevo_nombre.c_str()) == 0) {
            cout << "Archivo de logs renombrado a: " << nuevo_nombre << endl;
        } else {
            perror("Error al renombrar el archivo de logs");
        }
    }
}

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
    escribir_logs("Usuarios registrados en la tabla hash");
}

// crea y configura el socket del servidor
int crearSocketServidor(sockaddr_in& direccion){
    // se crea el socket
    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Error al crear el socket");
        escribir_logs("Error al crear el socket del servidor");
        return -1;
    }

    // se llena la estructura con los datos del servidor la ip y su puerto
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);
    direccion.sin_addr.s_addr = INADDR_ANY;

    // se bindea el socket con la dirección
    if (bind(socket_servidor, (sockaddr*)&direccion, sizeof(direccion)) < 0) {
        perror("Error al hacer bind");
        escribir_logs("Error al hacer bind del socket");
        return -1;
    }

    // se pone el socket en modo escucha
    if (listen(socket_servidor, 5) < 0) {
        perror("Error en listen");
        escribir_logs("Error al poner el socket en modo escucha");
        return -1;
    }
    escribir_logs("Socket servidor creado y escuchando en puerto " + to_string(PUERTO));
    return socket_servidor;
}

// aceptar a alguien que intenta conectase
void aceptarCliente(int socket_servidor, Lista& clientes, TablaHash& tabla) {
    sockaddr_in cliente_dir;
    socklen_t cliente_len = sizeof(cliente_dir);
    int socket_cliente = accept(socket_servidor, (sockaddr*)&cliente_dir, &cliente_len);

    if (socket_cliente < 0) {
        escribir_logs("Error al aceptar conexión de cliente");
        return;
    }
    
    if (clientes.ObtenerCantidad() < MAX_CLIENTES) {
        Cliente nuevoCliente;
        nuevoCliente.socket_fd = socket_cliente;

        if (nuevoCliente.socket_fd >= 0) {
            // Pedir usuario
            const char* usu = "Usuario: ";
            send(nuevoCliente.socket_fd, usu, strlen(usu), 0);
            recv(nuevoCliente.socket_fd, nuevoCliente.usuario, TAM, 0);

            // Pedir contraseña
            const char* contr = "Password: ";
            send(nuevoCliente.socket_fd, contr, strlen(contr), 0);
            recv(nuevoCliente.socket_fd, nuevoCliente.contraseña, TAM, 0);

            // Validar credenciales
            if (tabla.iniciarsesion(nuevoCliente.usuario, nuevoCliente.contraseña)==0) {
                const char* bienvenida = "¡Bienvenido al servidor!";
                send(nuevoCliente.socket_fd, bienvenida, strlen(bienvenida), 0);

                // **Ahora sí agregamos el cliente a la lista**
                clientes.insertarAlFinal(nuevoCliente);
                cout << "Cliente autenticado y conectado en la posición " << clientes.ObtenerCantidad() << endl;
                string usuario= nuevoCliente.usuario;
                escribir_logs("Cliente " + usuario + " autenticado correctamente");
                
            } else if (tabla.iniciarsesion(nuevoCliente.usuario, nuevoCliente.contraseña)==1) {
                const char* error = "El usuario ya está Online. Conexión cerrada.";
                send(nuevoCliente.socket_fd, error, strlen(error), 0);
                close(nuevoCliente.socket_fd);
                cout << "Cliente rechazado por estar ya conectado.\n";
                string usuario= nuevoCliente.usuario;
                escribir_logs("Cliente " + usuario + " rechazado: usuario ya online");
            } else {
                const char* error = "Usuario o contraseña incorrectos. Conexión cerrada.";
                send(nuevoCliente.socket_fd, error, strlen(error), 0);
                close(nuevoCliente.socket_fd);
                cout << "Cliente rechazado por credenciales incorrectas.\n";
                string usuario= nuevoCliente.usuario;
                escribir_logs("Intento fallido de conexión con usuario: " + usuario);
            }
        }
    } else {
        cout << "Servidor lleno, rechazando conexión." << endl;
        escribir_logs("Servidor lleno. Se rechazó una conexión entrante");
        close(socket_cliente);
    }
}


// recibir mensajes de cada cliente y reenviarlos a los demás (broadcast)
void manejarMensajes(Lista& clientes, TablaHash& tabla) {
    Nodo* actual = clientes.obtenerCabeza();
    while (actual != nullptr) {
        if (actual->dato.socket_fd != -1) {
            fd_set lectura_fds;
            FD_ZERO(&lectura_fds);
            FD_SET(actual->dato.socket_fd, &lectura_fds);
            timeval timeout = {0, 0}; // sin espera

            if (select(actual->dato.socket_fd + 1, &lectura_fds, NULL, NULL, &timeout) > 0) {
                ssize_t bytes = recv(actual->dato.socket_fd, actual->dato.mensaje, TAM, 0);
                if (bytes > 0) {
                    cout << "Cliente " << actual->dato.usuario << " dice: " << actual->dato.mensaje << endl;
                    string user=actual->dato.usuario, mensaje=actual->dato.mensaje;
                    string mensaje_completo = user + ": " + mensaje;
                    escribir_logs("Mensaje de " + mensaje_completo);
                    // Broadcast a otros clientes
                    Nodo* broad = clientes.obtenerCabeza();
                    while (broad != nullptr) {
                        if (broad != actual) {
                            send(broad->dato.socket_fd, mensaje_completo.c_str(), mensaje_completo.length(), 0);
                        }
                        broad = broad->siguiente;
                    }

                    memset(actual->dato.mensaje, 0, TAM);
                } else {
                    cout << "Cliente " << actual->dato.usuario << " desconectado.\n";
                    string usuario=actual->dato.usuario;
                    escribir_logs("Cliente " + usuario + " desconectado");
                    close(actual->dato.socket_fd);
                    Nodo* tem=actual->siguiente;
                    clientes.eliminarNodo(actual->dato);
                    tabla.Desconectar(actual->dato.usuario, actual->dato.contraseña);
                    actual = tem;
                    continue; 
                }
            }
        }
        actual = actual->siguiente;
    }
}


int main() {
    TablaHash tabla;
    registrarUsuarios(tabla); 

    Lista clientes;  

    sockaddr_in direccion;
    int socket_servidor = crearSocketServidor(direccion);
    if (socket_servidor < 0) return 1;

    cout << "Servidor escuchando en el puerto " << PUERTO << endl;
    escribir_logs("Servidor iniciado en el puerto " + to_string(PUERTO)); //to string nos esta convirtiendo los int a char
    // bucle principal, para escucha y respuesta
    while (true) {
        fd_set lectura_fds;
        FD_ZERO(&lectura_fds);

        // se añade el socket del servidor al conjunto de sockets (para nuevas conexiones)
        FD_SET(socket_servidor, &lectura_fds);

        // agrega la entrada del teclado
        FD_SET(STDIN_FILENO, &lectura_fds);

        // define el más grande entre socket y teclado
        int max_fd = socket_servidor > STDIN_FILENO ? socket_servidor : STDIN_FILENO;

        // se añaden todos los clientes conectados al conjunto de sockets (el compilado que yo les habia dicho)
        Nodo* actual = clientes.obtenerCabeza();
        while (actual != nullptr) {
            if (actual->dato.socket_fd != -1) {
                FD_SET(actual->dato.socket_fd, &lectura_fds);
                if (actual->dato.socket_fd > max_fd)
                    max_fd = actual->dato.socket_fd;
            }
            actual = actual->siguiente;
        }

        // timeout de espera (50ms)
        timeval timeout = {0, 50000};

        // select escucha todos los sockets a la vez sin bloquearse
        int actividad = select(max_fd + 1, &lectura_fds, NULL, NULL, &timeout);

        if (actividad < 0) {
            perror("Error en select");
            escribir_logs("Error en select en el bucle principal");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &lectura_fds)) {
            escribir_logs("Servidor cerrado");
            cin.ignore(); //limpiamos biffer
            escribir_logs("Servidor cerrado", true);
            close(socket_servidor);  // se cierra el servidor al terminar
            return 0;
        }

        // si hay actividad en el socket del servidor, es un nuevo cliente
        if (FD_ISSET(socket_servidor, &lectura_fds)) {
            aceptarCliente(socket_servidor, clientes, tabla);
        }

        // revisar si algún cliente envió mensaje
        manejarMensajes(clientes, tabla);
    }

    return 0;
}
