#include <array>

/**
 * Struct containing error code and reason 
*/
struct Error {
    short errorCode;
    char reason[128];
};

/**
 * @brief this is a method that will use the inputBuffer, which is supposed to be 
 * a STUN message, and create a appropiate STUN response and put it into the output buffer
 * that will be sent back to the client
 * @param inputBuffer buffer with the datagram package containing(hopefully) a STUN message
 * @param responseBuffer response buffer that will be sent back to client
 * @param clientAddress client socket info containing port and ip-address
*/
void handleSTUNMessage(std::array<char, 1024> inputBuffer, char *responeBuffer, int *responseSize, struct sockaddr_in clientAddress);

int isStunMessage(unsigned char byte);
int containsMagicCookie(std::array<char, 1024> &input);
int validTransactionID(std::array<char, 1024> &input);
int validMessageLength(std::array<char, 1024> &input);