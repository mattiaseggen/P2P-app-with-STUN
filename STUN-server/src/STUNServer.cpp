#include <sys/socket.h>
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
    int socketfd;
    char buffer[MAXLINE];
    struct sockaddr_in serverAddress;

public:
    STUNServer()
    {
        // Creating server socket
        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketfd < 0)
        {
            perror("Socket creation failed.");
            exit(EXIT_FAILURE);
        }

        memset(&serverAddress, 0, sizeof(serverAddress));

        // Configuring the server:
        serverAddress.sin_family = AF_INET; //IPv4
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(PORT);

        // Bind the socket with the server address
        if (bind(socketfd, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        {
            perror("Failed to bind socket and server address");
            exit(EXIT_FAILURE);
        }
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
            memset(buffer, 0, MAXLINE);
            bytes_read = recvfrom(socketfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&clientAddress, &len);
            buffer[bytes_read] = '\0';
            
            std::copy(std::begin(buffer), std::end(buffer), std::begin(bufferCopy));

            worker_threads.post([this, bufferCopy, clientAddress, len]{
                char response[MAXLINE];
                int responseSize;

                handleSTUNMessage(bufferCopy, response, &responseSize, clientAddress);
                sendto(this->socketfd, (const char *)response, responseSize, MSG_CONFIRM, (const struct sockaddr *)&clientAddress, len);
            });
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