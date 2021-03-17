

/**
 * @brief this is a method that will use the inputBuffer, which is supposed to be 
 * a STUN message, and create a appropiate STUN response and put it into the output buffer
 * that will be sent back to the client
 * @param inputBuffer buffer with the datagram package containing(hopefully) a STUN message
 * @param responseBuffer response buffer that will be sent back to client
 * @param clientAddress client socket info containing port and ip-address
*/
void handleSTUNMessage(char *inputBuffer, char *responeBuffer, struct sockaddr_in clientAddress);
