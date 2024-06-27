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
#include <ctime>
#include <bits/stdc++.h> 
#include <unistd.h> 

#define BUFFER_SIZE 1500

void serverTCP(int port, char* type);
void clientTCP(int port, const char* server_ip, int time,int interval, char* type);

void serverUDP(int port, char* type);
void clientUDP(int port, const char* server_ip, int time,int interval, char* type);

unsigned long long convertType(char* type);

int main(int argc,const char* argv[])
{
    bool serv;
    int port=8000;
    int time=10;
    int interval=1;
    char type[20] = "bytes";
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
            if(strcmp(argv[i],"k")==0)
                strcpy(type,"kilobits");
            else if(strcmp(argv[i],"K")==0)
                strcpy(type,"kilobytes");
            else if(strcmp(argv[i],"m")==0)
                strcpy(type,"megabits");
            else if(strcmp(argv[i],"M")==0)
                strcpy(type,"megabytes");
            else if(strcmp(argv[i],"g")==0)
                strcpy(type,"gigabits");
            else if(strcmp(argv[i],"G")==0)
                strcpy(type,"gigabytes");
            else
            {
                std::cout<<"option not available";
                return -1;
            }
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

unsigned long long convertType(char* type)
{
    if(!strcmp(type,"bytes"))
        return 1;
    else if(!strcmp(type, "kilobits"))
        return 125;
    else if(!strcmp(type, "kilobytes"))
        return 1000;
    else if(!strcmp(type, "megabits"))
        return 125000;
    else if(!strcmp(type, "megabytes"))
        return 1000000;
    else if(!strcmp(type, "gigabits"))
        return 125000000;
    else if(!strcmp(type, "gigabytes"))
        return 1000000000;
    return 0;
}

void serverTCP(int port, char* type) 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

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

    std::cout << "TCP Server is listening on port " << port << std::endl;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    int time;
    recv(new_socket, &time, sizeof(time), 0);
    int interval;
    recv(new_socket, &interval, sizeof(interval), 0);

    auto start = std::chrono::high_resolution_clock::now();
    auto next_interval = start + std::chrono::seconds(interval);
    unsigned long long total_received = 0;
    unsigned long long total_sent = 0;
    unsigned long long interval_sent = 0;
    unsigned long long interval_received = 0;
    int interval_start = 0;
    int interval_end = interval;
    unsigned long long divType = convertType(type);

    while (true) 
    {
        unsigned long long bytes_received = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_received < 0) {
            perror("Failed to receive data");
            break;
        }
        send(new_socket, &bytes_received , sizeof(bytes_received), 0);
        interval_sent+=1024;
        total_sent+=1024;
        interval_received += bytes_received;
        total_received += bytes_received;
        auto now = std::chrono::high_resolution_clock::now();

        if (now >= next_interval) 
        {
            std::chrono::duration<double> interval_elapsed = now - (next_interval - std::chrono::seconds(interval));
            double bandwidth = (interval_received * 8.0) / interval_elapsed.count();
            std::cout << "Interval: [" << interval_start << "-" << interval_end << "], Sent: " << interval_sent/divType
                      <<" "<< type <<", Received "<<interval_received/divType <<" "<< type <<", Bandwidth: " 
                      << bandwidth / (1000 * 1000) << " Mbps" << std::endl;
            interval_received = 0;
            interval_sent=0;
            interval_start+=interval;
            interval_end+=interval;
            next_interval += std::chrono::seconds(interval);
        }

        std::chrono::duration<double> total_elapsed = now - start;
        if ((int)total_elapsed.count() >= time)
        {
            double total_bandwidth = (total_received * 8.0) / total_elapsed.count();
            std::cout << "\nTotal sent: " <<total_sent<< " bytes, Total received: " << total_received 
                      << " bytes, Total Bandwidth: " << total_bandwidth / (1000 * 1000) << "Mbps" << std::endl;
            break;
        }
    }

    close(new_socket);
    close(server_fd);
}


void clientTCP(int port, const char* server_ip, int time, int interval, char* type)
{
    int sock = 0; // in bits per second
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
    std::cout<<"waiting for conncetion\n";
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return;
    }
    
    std::cout << "TCP Connected\n";

    send(sock, &time, sizeof(time), 0);
    send(sock, &interval, sizeof(interval), 0);

    auto start = std::chrono::high_resolution_clock::now();
    auto next_interval = start + std::chrono::seconds(interval);
    unsigned long long total_sent = 0;
    unsigned long long interval_sent = 0;
    unsigned long long interval_received = 0;
    unsigned long long total_received = 0;
    int interval_start = 0;
    int interval_end = interval;
    unsigned long long divType = convertType(type);

    while (true) 
    {
        unsigned long long bytes_sent = send(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_sent <= 0) {
            perror("Failed to send data");
            break;
        }
        unsigned long long bytes_received;
        recv(sock, &bytes_received, sizeof(bytes_received), 0);
        if (bytes_received <= 0) {
            perror("Failed to receive data");
            break;
        }
        interval_received+=bytes_received;
        total_received+=bytes_received;
        interval_sent += bytes_sent;
        total_sent += bytes_sent;
        auto now = std::chrono::high_resolution_clock::now();

        if (now >= next_interval) 
        {
            std::chrono::duration<double> interval_elapsed = now - (next_interval - std::chrono::seconds(interval));
            double bandwidth = (interval_received * 8.0) / interval_elapsed.count();
            std::cout << "Interval: [" << interval_start << "-" << interval_end << "], Sent: " << interval_sent/divType
                      <<" "<< type <<", Received "<<interval_received/divType <<" "<< type <<", Bandwidth: " 
                      << bandwidth / (1000 * 1000) << " Mbps" << std::endl;
            interval_sent = 0;
            interval_received = 0;
            interval_start+=interval;
            interval_end+=interval;
            next_interval += std::chrono::seconds(interval);
        }

        std::chrono::duration<double> total_elapsed = now - start;
        if (total_elapsed.count() >= time)
        {
            double total_bandwidth = (total_received * 8.0) / total_elapsed.count();
            std::cout << "\nTotal sent: " << total_sent << " bytes, Total received: " << total_received 
                      << " bytes, Total Bandwidth: " << total_bandwidth / (1000 * 1000) << " Mbps" << std::endl;
            break;
        }
    }

    close(sock);
}


void serverUDP(int port, char* type) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t addrlen = sizeof(cliaddr);
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

    std::cout << " UDP Server is listening on port " << port << std::endl;

    int time;
    recvfrom(sockfd, &time, sizeof(time), 0, (struct sockaddr *)&cliaddr, &addrlen);
    int interval;
    recvfrom(sockfd, &interval, sizeof(interval), 0, (struct sockaddr *)&cliaddr, &addrlen);

    auto start = std::chrono::high_resolution_clock::now();
    auto next_interval = start + std::chrono::seconds(interval);
    unsigned long long total_received = 0;
    unsigned long long total_sent = 0;
    unsigned long long interval_sent = 0;
    unsigned long long interval_received = 0;
    int interval_start = 0;
    int interval_end = interval;
    unsigned long long divType = convertType(type);

    while (true) 
    {
        unsigned long long bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &addrlen);
        if (bytes_received < 0) {
            perror("Failed to receive data");
            break;
        }
        sendto(sockfd, &bytes_received, sizeof(bytes_received), 0, (struct sockaddr *)&cliaddr, addrlen);
        interval_sent+=1024;
        total_sent+=1024;
        interval_received += bytes_received;
        total_received += bytes_received;
        auto now = std::chrono::high_resolution_clock::now();

        if (now >= next_interval) 
        {
            std::chrono::duration<double> interval_elapsed = now - (next_interval - std::chrono::seconds(interval));
            double bandwidth = (interval_received * 8.0) / interval_elapsed.count();
            std::cout << "Interval: [" << interval_start << "-" << interval_end << "], Sent: " << interval_sent/divType
                      <<" "<< type <<", Received "<<interval_received/divType <<" "<< type <<", Bandwidth: " 
                      << bandwidth / (1000 * 1000) << " Mbps" << std::endl;
            interval_received = 0;
            interval_sent=0;
            interval_start+=interval;
            interval_end+=interval;
            next_interval += std::chrono::seconds(interval);
        }

        std::chrono::duration<double> total_elapsed = now - start;
        if ((int)total_elapsed.count() >= time)
        {
            double total_bandwidth = (total_received * 8.0) / total_elapsed.count();
            std::cout << "\nTotal sent: " <<total_sent<< " bytes, Total received: " << total_received 
                      << " bytes, Total Bandwidth: " << total_bandwidth / (1000 * 1000) << "Mbps" << std::endl;
            break;
        }
    }


    close(sockfd);
}

void clientUDP(int port, const char* server_ip, int time, int interval, char* type) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;
    memset(buffer, 'a', sizeof(buffer));

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    

    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection Failed");
        return;
    }   

    std::cout<<"UDP Connected\n";

    sendto(sockfd, &time, sizeof(time), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    sendto(sockfd, &interval, sizeof(interval), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    auto start = std::chrono::high_resolution_clock::now();
    auto next_interval = start + std::chrono::seconds(interval);
    unsigned long long total_sent = 0;
    unsigned long long interval_sent = 0;
    unsigned long long interval_received = 0;
    unsigned long long total_received = 0;
    int interval_start = 0;
    int interval_end = interval;
    unsigned long long divType = convertType(type);

    while (true) 
    {
        int bytes_sent = sendto(sockfd, buffer, BUFFER_SIZE, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        if (bytes_sent < 0) {
            perror("Failed to send data");
            break;
        }

        unsigned long long bytes_received;
        recvfrom(sockfd, &bytes_received, sizeof(bytes_received), 0, nullptr, nullptr);

        if (bytes_received <= 0) {
            perror("Failed to receive data");
            break;
        }
        interval_received+=bytes_received;
        total_received+=bytes_received;
        interval_sent += bytes_sent;
        total_sent += bytes_sent;
        auto now = std::chrono::high_resolution_clock::now();

        if (now >= next_interval) 
        {
            std::chrono::duration<double> interval_elapsed = now - (next_interval - std::chrono::seconds(interval));
            double bandwidth = (interval_received * 8.0) / interval_elapsed.count();
            std::cout << "Interval: [" << interval_start << "-" << interval_end << "], Sent: " << interval_sent/divType
                      <<" "<< type <<", Received "<<interval_received/divType <<" "<< type <<", Bandwidth: " 
                      << bandwidth / (1000 * 1000) << " Mbps" << std::endl;
            interval_sent = 0;
            interval_received = 0;
            interval_start+=interval;
            interval_end+=interval;
            next_interval += std::chrono::seconds(interval);
        }

        std::chrono::duration<double> total_elapsed = now - start;
        if (total_elapsed.count() >= time)
        {
            double total_bandwidth = (total_received * 8.0) / total_elapsed.count();
            std::cout << "\nTotal sent: " << total_sent << " bytes, Total received: " << total_received 
                      << " bytes, Total Bandwidth: " << total_bandwidth / (1000 * 1000) << " Mbps" << std::endl;
            break;
        }
    }

}