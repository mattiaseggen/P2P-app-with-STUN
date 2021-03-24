#include "../inc/STUNOperations.hpp"
#include <assert.h>


void testIsStunMessage(std::array<char, 1024> &testInput){
    int result = isStunMessage(testInput[0]);
    assert(result == 1);
}

void testContainsMagicCookie(std::array<char, 1024> &testInput){
    int result = containsMagicCookie(testInput);
    assert(result == 1);
}

void testValidTransactionID(std::array<char, 1024> &testInput){
    int result = validTransactionID(testInput);
    assert(result == 1);
}

void testValidMessageLength(std::array<char, 1024> &testInput){
    int result = validMessageLength(testInput);
    assert(result == 1);
}

