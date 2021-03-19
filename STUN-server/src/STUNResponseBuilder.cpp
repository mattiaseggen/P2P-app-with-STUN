#include "../inc/STUNResponseBuilder.hpp"

STUNResponseBuilder& STUNResponseBuilder::addMessageType(uint16_t messageType){
    //Setting STUN messagetype:
    this->response.responseBuffer[0] = messageType >> 8;
    this->response.responseBuffer[1] = messageType & 255;

    return *this;
}

STUNResponseBuilder& STUNResponseBuilder::addMessageLength(uint16_t messageLength){
    //Setting STUN messagelength:
    this->response.responseBuffer[2] = messageLength >> 8;
    this->response.responseBuffer[3] = messageLength & 255;

    return *this;
}

STUNResponseBuilder& STUNResponseBuilder::addMagicCookie(){
    //Setting STUN magic cookie
    this->response.responseBuffer[4] = 0x21;
    this->response.responseBuffer[5] = 0x12;
    this->response.responseBuffer[6] = 0xA4;
    this->response.responseBuffer[7] = 0x42;

    return *this;
}

STUNResponseBuilder& STUNResponseBuilder::addTransactionID(char *inputBuffer){
    //Setting STUN transaction ID
    for (int i = 8; i < 20; i++)
    {
        this->response.responseBuffer[i] = inputBuffer[i];
    }

    return *this;
}

STUNResponseBuilder& STUNResponseBuilder::addAttributeHeader(uint16_t type, uint16_t length){

    // Setting attribute type
    this->response.responseBuffer[20] = type >> 8;
    this->response.responseBuffer[21] = type & 255;

    // Setting attribute length
    this->response.responseBuffer[22] = length >> 8;
    this->response.responseBuffer[23] = length & 255;

    return *this;
}

STUNResponseBuilder& STUNResponseBuilder::addXorMappedAddressIPv4Attribute(uint32_t ip, uint16_t port){
    // Setting family
    this->response.responseBuffer[24] = 0;
    this->response.responseBuffer[25] = 0x01; //IPv4 address
    
    // Setting port
    this->response.responseBuffer[26] = port >> 8;
    this->response.responseBuffer[27] = port & 255;

    // Setting IPv4 address
    this->response.responseBuffer[28] = ip >> 24;
    this->response.responseBuffer[29] = ip >> 16;
    this->response.responseBuffer[30] = ip >> 8;
    this->response.responseBuffer[31] = ip & 255;

    return *this;
}

STUNResponseBuilder& STUNResponseBuilder::addErrorAttribute(short error, char* reason){
    // reserved bits
    this->response.responseBuffer[24] = 0;
    this->response.responseBuffer[25] = 0;

    // Setting error class and error number
    short errorClass = error / 100;
    this->response.responseBuffer[26] = (char)errorClass;

    short errorNumb = error % 100;
    this->response.responseBuffer[27] = (char)errorNumb;

    // 128 because length of the reasons we have defined is no longer than 128
    for (int i = 0; i < 128; i++)
    {
        this->response.responseBuffer[28 + i] = reason[i];
    }

    return *this;
};