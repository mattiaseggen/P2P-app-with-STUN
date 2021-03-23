# STUN server

STUN-server name: **13.74.199.118:3478**
Link to "siste continous integration"????????????

## Introduction

This STUN server was created as a volunteer project to learn about the STUN server protocol and P2P communication. We used [RFC 5389 - Session Traversal Utilities for NAT](https://tools.ietf.org/html/rfc5389) as guidance when developing the STUN server. We chose to write the server in C++ because we have done the previous obligatory assignments in C++ aswell.

The first goal was to get a simple UDP server up and running that could recieve av STUN binding request and simply return a STUN success response containing the port and IPv4 address. Afterwards we worked on implementing more functionality like appropriate error responses if something in the STUN binding request was missing or invalid. A complete list of the implemented functionality can be found in the **Implemented Functionalites** section.

## Implemented functionality

The STUN-server is a UDP server that uses a Worker class that creates worker threads which handles every package that is recieved. Everytime the ```recvfrom(...)``` function recieves a STUN message, the processing and response of the message is posted as a task for the worker threads to handle.

The first, and most important, functionality that is implemented is **recieving STUN binding request and returning the port and IPv4 address in a STUN success response**. Other methods

## Future development

## External dependencies

We use **CMake** which is used for build automation, testing, including packages and installation of software. We have used it to compile our code as well as run tests and the program itself.

DOCker????



## Installation Guide

This is an installation guide that gives instructions on how to set up the STUN-server.

1. 

## How to start STUN-server

## How run tests

## API Documentation