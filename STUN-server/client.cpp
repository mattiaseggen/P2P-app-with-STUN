#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <iostream>

#define PORT 3478
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    char message[20] = {
        (char)0x00, (char)0x01, (char)0x00, (char)0x00,
        (char)0x21, (char)0x12, (char)0xA4, (char)0x42,
        (char)0xD9, (char)0xC0, (char)0x37, (char)0x6a,
        (char)0xC7, (char)0xDB, (char)0xB5, (char)0xD0,
        (char)0x28, (char)0x6F, (char)0x1F, (char)0x80
    };
    struct sockaddr_in servaddr;
      int n, len;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket creation failed");
        exit(0);
    }
  
    memset(&servaddr, 0, sizeof(servaddr));
  
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("13.74.199.118");
  
    if (connect(sockfd, (struct sockaddr*)&servaddr, 
                             sizeof(servaddr)) < 0) {
        printf("\n Error : Connect Failed \n");
    }
  
    memset(buffer, 0, sizeof(buffer));
    write(sockfd, message, 20);

    printf("Message from server: ");
    read(sockfd, buffer, 1024);
    for(int i = 0; i < 32; i ++){
        std::cout << (int)buffer[i] << std::endl;
    }
    close(sockfd);
}