# STUN server

STUN-server name: **13.74.199.118:3478**
Link to last continous integration/deployment: [temporary](https://github.com/mattiaseggen/P2P-app-with-STUN/actions/runs/681011190)

## Introduction

This STUN server was created as a volunteer project to learn about the STUN server protocol and P2P communication. We used [RFC 5389 - Session Traversal Utilities for NAT](https://tools.ietf.org/html/rfc5389) as guidance when developing the STUN server. We chose to write the server in C++ because we have done the previous obligatory assignments in C++ aswell.

The first goal was to get a simple UDP server up and running that could recieve av STUN binding request and simply return a STUN success response containing the port and IPv4 address. Afterwards we worked on implementing more functionality like appropriate error responses if something in the STUN binding request was missing or invalid. We also managed to accept both UDP and TCP. A complete list of the implemented functionality can be found in the **Implemented Functionality** section.

## Implemented functionality

The STUN-server is a UDP/TCP server that listens to both UDP and TCP requests. When a UDP package or TCP package is recieved, the package and response is handled in a worker thread when it is a UDP package, and a child process when it is a TCP package.

The most important functionality that is implemented is **recieving STUN binding request and returning the port and IPv4 address in a STUN success response**. 

We also have several methods in the ```STUNOperations.cpp``` which validates the STUN message, makes sure it follows the rules in section 6 in RFC 5389 and returns an approriate error if something is wrong:

1. Check if the first two bits in the STUN header is 0, if this isn't the case, the message is not a STUN message and the error code 401 is returned.
2. Check if the magic cookie is ```0x2112A442```, if not the error code 402 is returned.
3. Check if the transaction ID is a valid value, which is a value from the interval 0 .. 2^96 - 1. If this isn't the case, the error code 403 is returned.
4. Check if the two least significant bits in the length byte is zero because the length of a STUN message will never be between 0 or 4. If this isn't the case the error code 404
5. Check if the STUN message type is a binding request. If it is something else than a binding request, the error code 405 is returned

We use a method called ```createBindingErrorResponse()``` if any of these errors occurs. Here is a complete list of our error codes and corresponding reasons that is used in the ```ERROR_CODE``` attribute:

```C++
static struct Error errorList[5] = {
    {401, "This is not a STUN message. First two bits needs to be 0."},
    {402, "The magic cookie is either missing or not correct. It is supposed to be 0x2112A442."},
    {403, "The transaciton ID is either missing or not the correct value."},
    {404, "The length is either missing or an invalid value."},
    {405, "This STUN message type is either not valid or not implemented in this STUN-server"}
};
```

To create a binding error response or binding success response, we have created a ```STUNResponseBuilder``` class that uses the builder design pattern to create a STUN response. The STUNResponseBuilder takes in a response buffer that it fills with the help of builder methods like ```addMessageType()```, ```addAttributeHeader()``` and ```addMagicCookie()``` etc. View the STUNResponseBuilder class to see the rest of the builder methods.

All in all we have created a complete STUN server that does what a basic STUN server should do. However, if we had more time, there are several functionalities we would consider adding to make the STUN server even better!

## Future development

STUN-servers main functionality is to provide clients with server reflexive transport addresses by recieving and replying to STUN Binding requests. However there are also other functionalities and details that can be added to a STUN server. We only had a little more than one week to learn about and create a STUN server which means we did not have time to implement everything that is possible. However if we had more time, here is some of the things we could consider adding:

1. We can make sure the STUN server supports backwards compatibility with [RFC3489](https://tools.ietf.org/html/rfc3489) which would make it possible for old STUN clients to connect to our server. This means that we have to implement a way to detect if the Binding request message is sent from RFC 3489 and if so, take care of the missing magic cookie and use MAPPED-ADDRESS instead of XOR-MAPPED-ADDRESS.
2. We can implement a FINGERPRINT mechanism that helps deciding whether the message is a STUN message or not. However since we are creating a stand-alone server that only runs STUN, this would not provide any benefit.
3. We can provide DNS entries for the client to help them determine the IP address and port of the server.
4. Our STUN server does not accept IPv6-addresses, so this is definitly something we would consider adding in the future.
5. We can make the STUN server compatible with TLS-over-TCP.
6. We have implemented some error-handling as mentioned above, however we have not implemented a way to check if the request contains one or more unknown comprehension-required attributes. If this is the case, the server should reply with an error response with an error code of 420 (Unknown Attribute) in addition to including an UNKNOWN-ATTRIBUTES attribute which contains a list of all the unknown comprehension-required attributes.
7. We have not implemented authentication and message-integrity mehcansisms
8. We have not implemented an ALTERNATE-SERVER mechanism which allows a STUN server to redirect a clien to another server. This is something we could implement, but as of now, this is handled in the P2P application.

All of the above are missing features in our STUN server that we would consider implementing if we had more time.

## External dependencies

We use **CMake** which is used for build automation, testing, including packages and installation of software. We have used it to compile our code as well as run tests and the program itself.

We also use **docker** to run the tests and the STUN server itself. Docker is a tool to create containers on your PC that is running an OS of your choice. This is to make sure the PC running the STUN server does not run into any problems because of the OS.

The last external dependancy we use is **docker compose** which is a tool for defining and running multi-container Docker applications. We use it to build and run our docker images!


## Installation Guide

This is an installation guide that gives instructions on how to set up the STUN-server.

#### Clone the project

1. Clone this git repository [P2P-app-with-STUN](https://github.com/mattiaseggen/P2P-app-with-STUN)
2. CD into the project folder and then the "STUN-server".
3. Now you can proceed to the next section.

#### How to start STUN-server

There are two ways of starting the STUN server. The server is running when you get an output: ```Server is running. Waiting for STUN requests...```

1. Write ```make``` and then ```./STUNServer```. This will run the STUN server directly on your computer.
2. The other way is to simply write ```make run``` which will build the docker image and run it. The docker image will install all necessary dependencies, compile all necessary files, and run the server! To exit the docker and stop the server press ```CTRL + c``` **two** times.

## How to run tests

To run the test, make sure you are in the **STUN-server** folder in the project and write ```make test```
