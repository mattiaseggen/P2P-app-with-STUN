#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include "../inc/STUNOperations.hpp"

#define PORT 8080 // PORT, change this to correct STUN port
#define MAXLINE 1024 // MAX amount of bytes in datagram packet, change according to RFC


class STUNServer{
    private:

            // Socket file descriptor
            int socketfd;
            char buffer[MAXLINE];
            struct sockaddr_in serverAddress, clientAddress;

    public:

        STUNServer(){
            // Creating server socket
            socketfd = socket(AF_INET, SOCK_DGRAM, 0);
            if(socketfd < 0){
                perror("Socket creation failed.");
                exit(EXIT_FAILURE);
            }

            memset(&serverAddress, 0, sizeof(serverAddress));
            memset(&clientAddress, 0, sizeof(clientAddress));

            // Configuring the server:
            serverAddress.sin_family = AF_INET; //IPv4
            serverAddress.sin_addr.s_addr = INADDR_ANY;
            serverAddress.sin_port = htons(PORT);

            // Bind the socket with the server address
            if(bind(socketfd, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
                perror("Failed to bind socket and server address");
                exit(EXIT_FAILURE);
            }
        }
    
        // TODO: put it in while loop, make threads that handle each client
        void run(){
            std::cout << "Server is running. Waiting for STUN requests..." << std::endl;
            int bytes_read;
            socklen_t len = sizeof(clientAddress);//Length of clientaddres

            bytes_read = recvfrom(socketfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &clientAddress, &len);
            buffer[bytes_read] = '\0';

            std::cout << "Bytes read: " << bytes_read << std::endl;

            char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddress.sin_addr, str, INET6_ADDRSTRLEN);
            std::cout << "Client address: " << str << std::endl;

            char response[MAXLINE];
            handleSTUNMessage(buffer, response);
        }
};


int main(){

    STUNServer server;
    server.run();

    return 0;
}