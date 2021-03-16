

/**
 * @brief this is a method that will use the inputBuffer, which is supposed to be 
 * a STUN message, and create a appropiate STUN response and put it into the output buffer
 * that will be sent back to the client
 * @param inputBuffer buffer with the datagram package containing(hopefully) a STUN message
 * @param outputBuffer response buffer that will be sent back to client
*/
void handleSTUNMessage(char* inputBuffer, char* outputBuffer);
