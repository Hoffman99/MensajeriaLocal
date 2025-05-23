#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error al crear el socket\n";
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(25565);
    inet_pton(AF_INET, "192.168.72.9", &serv_addr.sin_addr);

    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error al conectar al servidor\n";
        return 1;
    }

    char buffer[256] = {0};
    ssize_t bytes_recibidos = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_recibidos > 0) {
        buffer[bytes_recibidos] = '\0';
        std::cout << "Mensaje recibido: " << buffer << "\n";
    } else {
        std::cerr << "Error al recibir mensaje\n";
    }

    close(sockfd);
    return 0;
}
