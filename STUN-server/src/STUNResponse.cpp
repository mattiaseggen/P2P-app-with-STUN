#include "../inc/STUNResponse.hpp"
#include "../inc/STUNResponseBuilder.hpp"
#include <iostream>


STUNResponseBuilder STUNResponse::create(char *responseBuffer){
    return STUNResponseBuilder{responseBuffer};
}
