#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include "../inc/STUNOperations.hpp"
#include "Workers.cpp"


#define PORT 3478    
#define MAXLINE 1024 

// TODO: make this one run with threads and put it in a while loop
// TODO: try to make ipv6 work as well

class STUNServer
{
private:
    // Socket file descriptor
    int udpfd, listenfd, connfd, nready, maxfdp1;
    char buffer[MAXLINE];
    pid_t childpid;
    fd_set rset;
    const int on = 1;
    struct sockaddr_in serverAddress;
    void sig_chld(int);

public:
    STUNServer()
    {
        // Create listening TCP socket
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0)
        {
            perror("Socket creation failed.");
            exit(EXIT_FAILURE);
        }
        bzero(&serverAddress, sizeof(serverAddress));

        // Configuring the server:
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(PORT);

        // binding server address struct to listenfd(tcp)
        if (bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        {
            perror("Failed to bind tcp socket and server address");
            exit(EXIT_FAILURE);
        }
        listen(listenfd, 10);

        // Creating udp server socket
        udpfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpfd < 0)
        {
            perror("Socket creation failed.");
            exit(EXIT_FAILURE);
        }
        // Bind the socket with the server address
        if (bind(udpfd, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        {
            perror("Failed to bind socket and server address");
            exit(EXIT_FAILURE);
        }

        FD_ZERO(&rset);

        maxfdp1 = max(listenfd, udpfd) + 1;
    }

    void run()
    {
        std::atomic_bool stop(false);
        
        std::cout << "Server is running. Waiting for STUN requests..." << std::endl;
        int bytes_read;

        Workers worker_threads(12);
        worker_threads.start();

        struct sockaddr_in clientAddress;
        memset(&clientAddress, 0, sizeof(clientAddress));
        socklen_t len = sizeof(clientAddress); //Length of clientaddres
        
        // Creating a array that can will be a copy of the buffer since regular arrays
        // can't be sent as copies, which is necessary when using thread.
        std::array<char, MAXLINE> bufferCopy;

        while(!stop){

            FD_SET(listenfd, &rset);
            FD_SET(udpfd, &rset);

            nready = select(maxfdp1, &rset, NULL, NULL, NULL);

            if(FD_ISSET(listenfd, &rset)){
                len = sizeof(clientAddress);
                connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &len);

                if((childpid = fork()) == 0){
                    close(listenfd);
                    memset(buffer, 0, MAXLINE);
                    bytes_read = read(connfd, buffer, sizeof(buffer));
                    std::cout << "Bytes read: " << bytes_read << std::endl;
                    std::copy(std::begin(buffer), std::end(buffer), std::begin(bufferCopy));

                    worker_threads.post([this, bufferCopy, clientAddress, len]{
                        char response[MAXLINE];
                        int responseSize;

                        handleSTUNMessage(bufferCopy, response, &responseSize, clientAddress);
                        for(int i = 0; i < 32; i ++){
                            std::cout << (int)response[i] << std::endl;
                        }
                        write(connfd, (const char*)response, responseSize);
                        //sendto(this->udpfd, (const char *)response, responseSize, MSG_CONFIRM, (const struct sockaddr *)&clientAddress, len);
                    });
                    close(connfd);
                    exit(0);
                }
                close(connfd);

                char str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddress.sin_addr, str, INET_ADDRSTRLEN);
                std::cout << str << std::endl;
            }
            
            if(FD_ISSET(udpfd, &rset)){
                len = sizeof(clientAddress);
                memset(buffer, 0, MAXLINE);
                bytes_read = recvfrom(udpfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&clientAddress, &len);

                buffer[bytes_read] = '\0';
            
                std::copy(std::begin(buffer), std::end(buffer), std::begin(bufferCopy));

                worker_threads.post([this, bufferCopy, clientAddress, len]{
                    char response[MAXLINE];
                    int responseSize;

                    handleSTUNMessage(bufferCopy, response, &responseSize, clientAddress);
                    sendto(this->udpfd, (const char *)response, responseSize, MSG_CONFIRM, (const struct sockaddr*)&clientAddress, len);
                });
            }            
        }

        worker_threads.stop();
        std::cout << "Server stopped" << std::endl;
    }
};

int main()
{
    
    STUNServer server;
    server.run();

    return 0;
}