#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#define BUFFER_SIZE 1024

void serverTCP(int port,int type);
void clientTCP(int port, const char* server_ip, int time,int interval, int type);

void serverUDP(int port,int type);
void clientUDP(int port, const char* server_ip, int time,int interval, int type);

int main(int argc,const char* argv[])
{
    bool serv;
    int port=8000;
    int time=10;
    int interval=1;
    int type=1;
    bool udp=0;
    const char* server_ip;
    int i=0;
    if(strcmp(argv[1],"-s")==0)
    {
        serv=1;
        i=2;
    }
    else if(strcmp(argv[1],"-c")==0)
    {
        serv=0;
        server_ip=argv[2];
        i=3;
    }
    else 
    {
        std::cout<<"Usage: ./speed -s [...] or ./speed -c [host address] [...]"<<std::endl;
        return -1;
    }
    while(i<argc)
    {
        if(strcmp(argv[i],"-p")==0)
        {
            port=atoi(argv[++i]);
        }
        else if(strcmp(argv[i],"-t")==0)
        {
            time=atoi(argv[++i]);
        }
        else if(strcmp(argv[i],"-i")==0)
        {
            interval=atoi(argv[++i]);
        }
        else if(strcmp(argv[i],"-u")==0)
        {
            udp=1;
        }
        else if(strcmp(argv[i],"-f")==0)
        {
            i++;
            if(strcmp(argv[i],"B")==0)
                type=8;
            else if(strcmp(argv[i],"k")==0)
                type=1000;
            else if(strcmp(argv[i],"K")==0)
                type=8000;
            else if(strcmp(argv[i],"m")==0)
                type=1000000;
            else if(strcmp(argv[i],"-M")==0)
                type=8000000;
        }
        else
        {
            std::cout<<"option not available"<<std::endl;
            return -1;
        }
        i++;
    }
    if(udp==0)
    {   
        if(serv==1)
        {
        serverTCP(port, type);
        }
        else clientTCP(port,server_ip,time,interval,type);
    }
    else if(udp==1)
    {   
        if(serv==1)
        {
        serverUDP(port, type);
        }
        else clientUDP(port,server_ip,time,interval,type);
    }
    return 0;
}

void serverTCP(int port, int type) 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *welcome_msg = "Welcome to the server!";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << port << std::endl;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    send(new_socket, welcome_msg, strlen(welcome_msg), 0);
    std::cout << "Welcome message sent" << std::endl;

    close(new_socket);
    close(server_fd);
}

void clientTCP(int port, const char* server_ip, int time, int interval, int type)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    memset(buffer, 'A', BUFFER_SIZE);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {   
        perror("Socket creation error");
        return;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return;
    }

    int valread = read(sock, buffer, BUFFER_SIZE);
    std::cout << "Received: " << buffer << std::endl;

    close(sock);
}

void serverUDP(int port, int type) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    const char *welcome_msg = "Welcome to the server!";


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << port << std::endl;

    sendto(sockfd, welcome_msg, strlen(welcome_msg), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, sizeof(cliaddr));
    std::cout << "Response sent to client." << std::endl;

    close(sockfd);
}

void clientUDP(int port, const char* server_ip, int time, int interval, int type) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    
    std::string hello = "Hello from client";
    sendto(sockfd, hello.c_str(), hello.size(), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    std::cout << "Hello message sent." << std::endl;

    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, nullptr, nullptr);
    buffer[n] = '\0';
    std::cout << "Server: " << buffer << std::endl;

    close(sockfd);
}
