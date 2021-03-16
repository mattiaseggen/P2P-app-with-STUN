#include "../inc/STUNOperations.hpp"
#include <iostream> // temporary for testing

int isStunMessage(unsigned char byte){
    if(byte >> 6 == 0b00){
        return 1;
    }
    return 0;
}

void handleSTUNMessage(char* inputBuffer, char* outputBuffer){
    // Check if the datagram package really is a STUN message
    if(isStunMessage((unsigned char)inputBuffer[0]) == 0){
        // TODO: create STUN error response
        std::cout << "This is not a STUN message" << std::endl;
    }

    // TODO: DECODE THE REST OF THE STUN message
}