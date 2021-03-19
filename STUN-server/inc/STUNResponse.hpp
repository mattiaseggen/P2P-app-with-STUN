#pragma once

class STUNResponseBuilder;

class STUNResponse{

    char *responseBuffer;

    STUNResponse(char *buffer) : responseBuffer(buffer) {}


    public:
        friend class STUNResponseBuilder;
        static STUNResponseBuilder create(char *responseBuffer);
};