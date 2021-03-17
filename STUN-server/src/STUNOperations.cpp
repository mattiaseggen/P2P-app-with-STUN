#include "../inc/STUNOperations.hpp"
#include <iostream> // temporary for testing
#include <math.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |0 0|     STUN Message Type     |         Message Length        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                         Magic Cookie                          |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                     Transaction ID (96 bits)                  |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

                  Figure 2: Format of STUN Message Header
*/

/** 
                        0                 1
                        2  3  4 5 6 7 8 9 0 1 2 3 4 5

                       +--+--+-+-+-+-+-+-+-+-+-+-+-+-+
                       |M |M |M|M|M|C|M|M|M|C|M|M|M|M|
                       |11|10|9|8|7|1|6|5|4|0|3|2|1|0|
                       +--+--+-+-+-+-+-+-+-+-+-+-+-+-+

                Figure 3: Format of STUN Message Type Field
*/

/**
 * The STUN message class is defined by C1 and C0.
 * 0b00 is a request, 0b01 is an indication, 0b10 is a success response
 * 0b11 is an error response. The bits at the M position defines the STUN
 * message method. The method we are using is binding which has the value:
 * 0b000000000001. Below I will define the different STUN message types:
*/

#define BINDING_REQUEST 0x0001
#define BINDING_SUCCESS_RESPONSE 0x0101
#define BINDING_ERROR_RESPONSE 0x0111
#define MAGIC_COOKIE 0x2112A442
// TODO: Add other type, maybe indication
#define MAGIC_COOKIE_OFFSET 4
#define MAXLINE 1024 // MAX amount of bytes in datagram packet, change according to RFC

int isStunMessage(unsigned char byte)
{
    if (byte >> 6 == 0b00)
    {
        return 1;
    }
    return 0;
}

int containsMagicCookie(char *input)
{
    unsigned char expectedVal[4] = {0x21, 0x12, 0xA4, 0x42};

    for (int i = 0; i < sizeof(expectedVal) / sizeof(expectedVal[0]); i++)
    {
        if (expectedVal[i] != (unsigned char)input[MAGIC_COOKIE_OFFSET + i])
        {
            return 0;
        }
    }
    return 1;
}

int validTransactionID(char *input)
{
    typedef unsigned char u8;
    unsigned transactionID = ((u8)input[8] << 24) | ((u8)input[9] << 16) | ((u8)input[10] << 8) | (u8)input[11];
    if (transactionID >= 0 && transactionID <= (79228163 * pow(10, 21)) - 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int validMessageLength(char *input)
{
    typedef unsigned char u8;
    unsigned length = ((u8)input[2] << 8) | (u8)input[3];
    if (length >= 0 || length <= (MAXLINE - 20))
    { // -20 because the the STUN header is 20 bytes long
        return 1;
    }
    else
    {
        return 0;
    }
}

void getClientPort(char *buffer, struct sockaddr_in clientAddress)
{
    uint16_t port;
    port = htons(clientAddress.sin_port);

    uint16_t XOR_port = port ^ 0x2112;

    buffer[0] = XOR_port >> 8;
    buffer[1] = XOR_port & 255;
}

void getClientIPv4(char *buffer, struct sockaddr_in clientAddress)
{
    uint32_t ip = ntohl(clientAddress.sin_addr.s_addr);
    uint32_t XOR_ip = ip ^ MAGIC_COOKIE;

    buffer[0] = XOR_ip >> 24;
    buffer[1] = (XOR_ip >> 16) & 255;
    buffer[2] = (XOR_ip >> 8) & 255;
    buffer[3] = XOR_ip & 255;
}

void createBindingSuccessResponse(char *input, char *responseBuffer, struct sockaddr_in clientAddress)
{ //Double check if this is the correct way for responseBuffer
    //Setting STUN messagetype:
    responseBuffer[0] = BINDING_SUCCESS_RESPONSE >> 8;
    responseBuffer[1] = BINDING_SUCCESS_RESPONSE & 255;

    //Setting STUN messagelength:
    responseBuffer[2] = 0;
    responseBuffer[3] = 12;

    //Setting STUN magic cookie
    responseBuffer[4] = 0x21;
    responseBuffer[5] = 0x12;
    responseBuffer[6] = 0xA4;
    responseBuffer[7] = 0x42;

    //Setting STUN transaction ID
    for (int i = 8; i < 20; i++)
    {
        responseBuffer[i] = input[i];
    }

    //Add XOR-MAPPED-ADDRESS attribute
    // setting attribute type
    responseBuffer[20] = 0;
    responseBuffer[21] = 0x20;
    // setting attribute length
    responseBuffer[22] = 0;
    responseBuffer[23] = 8;
    // setting attribute value
    responseBuffer[24] = 0;
    responseBuffer[25] = 0x01; //Ipv4 address

    char port[2];
    getClientPort(port, clientAddress);
    responseBuffer[26] = port[0];
    responseBuffer[27] = port[1];

    char ip[4];
    getClientIPv4(ip, clientAddress);
    for (int i = 28; i < 32; i++)
    {
        responseBuffer[i] = ip[i - 28];
    }
}

void handleSTUNMessage(char *inputBuffer, char *responseBuffer, struct sockaddr_in clientAddress)
{
    // Check if the datagram package really is a STUN message
    if (isStunMessage((unsigned char)inputBuffer[0]) == 0)
    {
        // TODO: create STUN error response
        std::cout << "This is not a STUN message, two first bits are not zero!" << std::endl;
    }

    // Check if the datagram package contains magic cookie
    if (containsMagicCookie(inputBuffer) == 0)
    {
        // TODO: create STUN error response
        std::cout << "This is not a STUN message, missing magic cookie!" << std::endl;
    }

    // Check if the transaction id is within its limit
    if (validTransactionID(inputBuffer) == 0)
    {
        // TODO: create STUN error response
        std::cout << "This transactionID is not valid!" << std::endl;
    }

    // check if hte message length is valid
    if (validMessageLength(inputBuffer) == 0)
    {
        // TODO: create STUN error response
        std::cout << "This length is not valid!" << std::endl;
    }

    //----------- handle message type -----------//
    if ((unsigned)inputBuffer[0] << 8 | inputBuffer[1] == BINDING_REQUEST)
    {
        std::cout << "This is a binding request!" << std::endl;
        createBindingSuccessResponse(inputBuffer, responseBuffer, clientAddress);
    }
}