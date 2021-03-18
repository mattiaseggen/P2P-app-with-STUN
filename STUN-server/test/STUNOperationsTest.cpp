#include "../src/STUNOperations.cpp"
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

int main()
{

    char testInput1[20] = {
        0x00, 0x01, 0x00, 0x00, 
        0x21, 0x12, 0xa4, 0x42, 
        0x79, 0x6d, 0x6d, 0x78, 
        0x68, 0x57, 0x44, 0x4d, 
        0x35, 0x64, 0x79, 0x2b
    };

    testIsStunMessage(testInput1);
    testContainsMagicCookie(testInput1);
    testValidTransactionID(testInput1);
    testValidMessageLength(testInput1);

    char testInput2[20] = {
        0xC0, 0x01, 0x00, 0x00, 
        0x21, 0x12, 0xa4, 0x42, 
        0x79, 0x6d, 0x6d, 0x78, 
        0x68, 0x57, 0x44, 0x4d, 
        0x35, 0x64, 0x79, 0x2b
    };

    return 0;
}