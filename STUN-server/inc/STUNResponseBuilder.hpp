#pragma once
#include "STUNResponse.hpp"
#include <stdint.h>
#include <utility>
#include <array>

#define MAXLINE 1024


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
        STUNResponseBuilder& addTransactionID(std::array<char, MAXLINE> &inputBuffer);
        STUNResponseBuilder& addAttributeHeader(uint16_t type, uint16_t length);
        STUNResponseBuilder& addXorMappedAddressIPv4Attribute(uint32_t ip, uint16_t port);
        STUNResponseBuilder& addErrorAttribute(short error, char *reason);

};