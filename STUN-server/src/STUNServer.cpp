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


class STUNServer
{
private:
    // file descriptors
    int udpfd, listenfd, connfd, nready, maxfdp1;
    char buffer[MAXLINE];
    pid_t childpid;  // Contains the process id
    fd_set rset; // Represents the file descriptor sets for the select function
    struct sockaddr_in serverAddress;  // Server address containing information about the server
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
        memset(&serverAddress, 0, sizeof(serverAddress));

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

        FD_ZERO(&rset); // Clears the file descriptor set

        maxfdp1 = max(listenfd, udpfd) + 1; // Finds the max amount of file descriptors
    }

    void run()
    {
        // NB! A way to actually stop the server correctly is still to be implemented
        std::atomic_bool stop(false);
        
        std::cout << "Server is running. Waiting for STUN requests..." << std::endl;
        int bytes_read;

        // Creates a worker class with 12 threads, can be adjusted to as many threads as we want
        Workers worker_threads(12);
        worker_threads.start(); // Start the worker threads so they are waiting for tasks to be posted

        // Address information like IPv4-address and port will be stored in this variable
        struct sockaddr_in clientAddress;
        memset(&clientAddress, 0, sizeof(clientAddress));
        socklen_t len = sizeof(clientAddress); // Size of clientaddres struct
        
        // Creating a array that can will be a copy of the buffer since regular arrays
        // can't be sent as copies, which is necessary when using thread.
        std::array<char, MAXLINE> bufferCopy;

        while(!stop){

            FD_SET(listenfd, &rset);
            FD_SET(udpfd, &rset);

            nready = select(maxfdp1, &rset, NULL, NULL, NULL);

            // Listens for TCP packages
            if(FD_ISSET(listenfd, &rset)){
                len = sizeof(clientAddress);
                connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &len);

                // Creates a fork and makes sure the process id is the child
                if((childpid = fork()) == 0){
                    close(listenfd);
                    memset(buffer, 0, MAXLINE);
                    bytes_read = read(connfd, buffer, sizeof(buffer));
                    std::copy(std::begin(buffer), std::end(buffer), std::begin(bufferCopy));

                    char response[MAXLINE];
                    int responseSize;

                    handleSTUNMessage(bufferCopy, response, &responseSize, clientAddress);
                    write(connfd, (const char*)response, responseSize);
                    close(connfd);
                    exit(0);
                }
                close(connfd);
            }
            
            // Listens for UDP package
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

        // Stop working threads
        worker_threads.stop();
        std::cout << "Server stopped" << std::endl;
    }
};

int main()
{
    // Create and start server
    STUNServer server;
    server.run();

    return 0;
}