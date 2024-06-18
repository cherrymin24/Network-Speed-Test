#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server IP address>" << std::endl;
        return -1;
    }
    int PORT=atoi(argv[2]);
    int TIME=atoi(argv[3]);

    const char *server_ip = argv[1];
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    read(sock, buffer, BUFFER_SIZE);
    std::cout << "Message from server: " << buffer << std::endl;

    close(sock);
    return 0;
}
