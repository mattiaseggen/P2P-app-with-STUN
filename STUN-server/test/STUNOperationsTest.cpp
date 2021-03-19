#include "../inc/STUNOperations.hpp"
#include <assert.h>


void testIsStunMessage(char *testInput){
    int result = isStunMessage(testInput[0]);
    assert(result == 1);
}

void testContainsMagicCookie(char *testInput){
    int result = containsMagicCookie(testInput);
    assert(result == 1);
}

void testValidTransactionID(char *testInput){
    int result = validTransactionID(testInput);
    assert(result == 1);
}

void testValidMessageLength(char *testInput){
    int result = validMessageLength(testInput);
    assert(result == 1);
}


// TODO: make better/more tests

