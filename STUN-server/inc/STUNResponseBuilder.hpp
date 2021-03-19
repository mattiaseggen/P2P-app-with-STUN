#pragma once
#include "STUNResponse.hpp"
#include <stdint.h>
#include <utility>


class STUNResponseBuilder{
    STUNResponse response;
    

    public:
        STUNResponseBuilder(char *responseBuffer) : response(responseBuffer) {}


        operator STUNResponse() const { 
            return std::move(response);
        }

        STUNResponseBuilder& addMessageType(uint16_t messageType);
        STUNResponseBuilder& addMessageLength(uint16_t messageLength);
        STUNResponseBuilder& addMagicCookie();
        STUNResponseBuilder& addTransactionID(char *inputBuffer);
        STUNResponseBuilder& addAttributeHeader(uint16_t type, uint16_t length);
        STUNResponseBuilder& addXorMappedAddressIPv4Attribute(uint32_t ip, uint16_t port);
        STUNResponseBuilder& addErrorAttribute(short error, char *reason);

};