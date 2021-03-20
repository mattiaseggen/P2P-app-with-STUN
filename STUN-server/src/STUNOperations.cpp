#include "../inc/STUNOperations.hpp"
#include "../inc/STUNResponse.hpp"
#include "../inc/STUNResponseBuilder.hpp"
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
#define MAXLINE 1024
#define SUCCESS_RESPONSE_SIZE 32
#define ERROR_RESPONSE_SIZE 156

static struct Error errorList[5] = {
    {401, "This is not a STUN message. First two bits needs to be 0."},
    {402, "The magic cookie is either missing or not correct. It is supposed to be 0x2112A442."},
    {403, "The transaciton ID is either missing or not the correct value."},
    {404, "The length is either missing or an invalid value."},
    {405, "This STUN message type is either not valid or not implemented in this STUN-server"}};

int isStunMessage(unsigned char byte)
{
    if (byte >> 6 == 0b00)
    {
        return 1;
    }
    return 0;
}

int containsMagicCookie(std::array<char, MAXLINE> &input)
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

int validTransactionID(std::array<char, MAXLINE> &input)
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
// CHECK LEAST SIGNIFICANT BITS IN LENGTH
int validMessageLength(std::array<char, MAXLINE> &input)
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

void createBindingSuccessResponse(std::array<char, MAXLINE> &input, char *responseBuffer, struct sockaddr_in clientAddress)
{
    STUNResponse::create(responseBuffer)
        .addMessageType(BINDING_SUCCESS_RESPONSE)
        .addMessageLength(12)
        .addMagicCookie()
        .addTransactionID(input)
        .addAttributeHeader(XOR_MAPPED_ADDRESS, 8)
        .addXorMappedAddressIPv4Attribute(ntohl(clientAddress.sin_addr.s_addr), htons(clientAddress.sin_port));
}

void createBindingErrorResponse(std::array<char, MAXLINE> &input, char *responseBuffer, struct Error error)
{
    STUNResponse::create(responseBuffer)
        .addMessageType(BINDING_ERROR_RESPONSE)
        .addMessageLength(136)
        .addMagicCookie()
        .addTransactionID(input)
        .addAttributeHeader(ERROR_CODE, 132)
        .addErrorAttribute(error.errorCode, error.reason);
}

int validSTUNMessage(std::array<char, MAXLINE> &inputBuffer, char *responseBuffer)
{
    if (isStunMessage((unsigned char)inputBuffer[0]) == 0)
    {
        std::cout << "This is not a STUN message, two first bits are not zero!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[0]);
        return 0;
    }
    else if (containsMagicCookie(inputBuffer) == 0)
    {
        std::cout << "This is not a STUN message, missing magic cookie!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[1]);
        return 0;
    }
    else if (validTransactionID(inputBuffer) == 0)
    {
        std::cout << "This transactionID is not valid!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[2]);
        return 0;
    }
    else if (validMessageLength(inputBuffer) == 0)
    {
        std::cout << "This length is not valid!" << std::endl;
        createBindingErrorResponse(inputBuffer, responseBuffer, errorList[3]);
        return 0;
    }

    return 1;
}

void handleSTUNMessage(std::array<char, MAXLINE> inputBuffer, char *responseBuffer, int *responseSize, struct sockaddr_in clientAddress)
{

    if (validSTUNMessage(inputBuffer, responseBuffer) == 1)
    {

        if ((unsigned)inputBuffer[0] << 8 | inputBuffer[1] == BINDING_REQUEST)
        {
            *responseSize = SUCCESS_RESPONSE_SIZE;
            createBindingSuccessResponse(inputBuffer, responseBuffer, clientAddress);
        }
        else
        {
            *responseSize = ERROR_RESPONSE_SIZE;
            createBindingErrorResponse(inputBuffer, responseBuffer, errorList[4]);
        }
    }
    else
    {
        *responseSize = ERROR_RESPONSE_SIZE;
        std::cout << "error occured" << std::endl;
    }
}