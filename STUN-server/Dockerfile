FROM ubuntu:latest
COPY . /usr/src/STUN-server
WORKDIR /usr/src/STUN-server
RUN apt-get update && \
    apt-get -y install g++
RUN g++ -o STUNServer src/STUNServer.cpp src/STUNOperations.cpp \
    src/STUNResponseBuilder.cpp src/STUNResponse.cpp -lpthread
CMD ["./STUNServer", "&", "&&", "echo", "done"]