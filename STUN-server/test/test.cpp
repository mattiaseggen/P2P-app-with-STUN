#include "STUNOperationsTest.cpp"
#include <iostream>

int main()
{

    char testInput1[20] = {
        (char)0, (char)1, (char)0, (char)0, 
        (char)33, (char)18, (char)164, (char)66, 
        (char)121, (char)109, (char)109, (char)120, 
        (char)104, (char)87, (char)68, (char)77, 
        (char)53, (char)100, (char)121, (char)43
    };

    testIsStunMessage(testInput1);
    testContainsMagicCookie(testInput1);
    testValidTransactionID(testInput1);
    testValidMessageLength(testInput1);



    std::cout << "All tests was passed!" << std::endl;

    return 0;
}