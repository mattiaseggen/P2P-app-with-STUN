# STUN server

STUN-server name: **13.74.199.118:3478**
Link to last continous integration/deployment: [temporary](https://github.com/mattiaseggen/P2P-app-with-STUN/actions/runs/681011190)

## Introduction

This STUN server was created as a volunteer project to learn about the STUN server protocol and P2P communication. We used [RFC 5389 - Session Traversal Utilities for NAT](https://tools.ietf.org/html/rfc5389) as guidance when developing the STUN server. We chose to write the server in C++ because we have done the previous obligatory assignments in C++ aswell.

The first goal was to get a simple UDP server up and running that could recieve av STUN binding request and simply return a STUN success response containing the port and IPv4 address. Afterwards we worked on implementing more functionality like appropriate error responses if something in the STUN binding request was missing or invalid. A complete list of the implemented functionality can be found in the **Implemented Functionalites** section.

## Implemented functionality

The STUN-server is a UDP server that uses a Worker class that creates worker threads which handles every package that is recieved. Everytime the ```recvfrom(...)``` function recieves a STUN message, the processing and response of the message is posted as a task for the worker threads to handle.

The most important functionality that is implemented is **recieving STUN binding request and returning the port and IPv4 address in a STUN success response**. 

We also have several methods in the ```STUNOperations.cpp``` which validates the STUN message:
1. 


## Future development

## External dependencies

We use **CMake** which is used for build automation, testing, including packages and installation of software. We have used it to compile our code as well as run tests and the program itself.

We also use **docker** to run the tests and the STUN server itself. This is to make sure the PC running the STUN server does not run into any problems because of the OS.


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

## API Documentation