#include "../inc/STUNOperations.hpp"
#include <iostream> // temporary for testing
#include <math.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iomanip>

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

#define XOR_MAPPED_ADDRESS 0x0020
#define ERROR_CODE 0x0009

#define MAGIC_COOKIE_OFFSET 4
#define MAXLINE 1024 // MAX amount of bytes in datagram packet, change according to RFC

static struct Error errorList[4] = {
    {401, "This is not a STUN message. First two bits needs to be 0."},
    {402, "The magic cookie is either missing or not correct. It is supposed to be 0x2112A442."},
    {403, "The transaciton ID is either missing or not the correct value."},
    {404, "The length is either missing or an invalid value."}};

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

void createSTUNHeader(char *input, char *responseBuffer, short messageType, short length)
{
    //Setting STUN messagetype:
    responseBuffer[0] = messageType >> 8;
    responseBuffer[1] = messageType & 255;

    //Setting STUN messagelength:
    responseBuffer[2] = length >> 8;
    responseBuffer[3] = length & 255;

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
}

void createAttributeHeader(char *responseBuffer, short attributeType, short length)
{
    // Setting attribute type
    responseBuffer[20] = attributeType >> 8;
    responseBuffer[21] = attributeType & 255;

    // setting attribute length
    responseBuffer[22] = length >> 8;
    responseBuffer[23] = length & 255;
}

void createErrorAttribute(char *responseBuffer, short error, char *reason)
{
    // reserved bits
    responseBuffer[24] = 0;
    responseBuffer[25] = 0;

    short errorClass = error / 100;
    responseBuffer[26] = (char)errorClass;

    short errorNumb = error % 100;
    responseBuffer[27] = (char)errorNumb;

    // 128 because length of the reasons we have defined is no longer than 128
    for (int i = 0; i < 128; i++)
    {
        responseBuffer[28 + i] = reason[i];
    }
}

void createBindingSuccessResponse(char *input, char *responseBuffer, struct sockaddr_in clientAddress)
{
    // Creating stun header with binding success response message type
    createSTUNHeader(input, responseBuffer, BINDING_SUCCESS_RESPONSE, 12);

    // Add XOR-MAPPED-ADDRESS attribute
    createAttributeHeader(responseBuffer, XOR_MAPPED_ADDRESS, 8);
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

void createBindingErrorResponse(char *inputBuffer, char *responseBuffer, struct Error error)
{
    // Creating stun header with binding error response message type
    createSTUNHeader(inputBuffer, responseBuffer, BINDING_ERROR_RESPONSE, 136);
    createAttributeHeader(responseBuffer, ERROR_CODE, 132);
    createErrorAttribute(responseBuffer, error.errorCode, error.reason);
}

int validateSTUNMessage(char *inputBuffer, char *responseBuffer)
{
    // Check if the datagram package really is a STUN message
    if (isStunMessage((unsigned char)inputBuffer[0]) == 0)
    {
        std::cout << "This is not a STUN message, two first bits are not zero!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[0]);
        return 0;
    }
    // Check if the datagram package contains magic cookie
    else if (containsMagicCookie(inputBuffer) == 0)
    {
        std::cout << "This is not a STUN message, missing magic cookie!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[1]);
        return 0;
    }
    // Check if the transaction id is within its limit
    else if (validTransactionID(inputBuffer) == 0)
    {
        std::cout << "This transactionID is not valid!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[2]);
        return 0;
    }
    // check if hte message length is valid
    else if (validMessageLength(inputBuffer) == 0)
    {
        std::cout << "This length is not valid!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[3]);
        return 0;
    }

    return 1;
}

void handleSTUNMessage(char *inputBuffer, char *responseBuffer, int *responseSize, struct sockaddr_in clientAddress)
{

    if (validateSTUNMessage(inputBuffer, responseBuffer) == 1)
    {
        if ((unsigned)inputBuffer[0] << 8 | inputBuffer[1] == BINDING_REQUEST)
        {
            std::cout << "This is a binding request!" << std::endl;
            createBindingSuccessResponse(inputBuffer, responseBuffer, clientAddress);
            *responseSize = 32;
        }
    }
    else
    {
        *responseSize = 156;
        std::cout << "error occured" << std::endl;
    }
}