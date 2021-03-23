FROM alpine:3.7
COPY ./ /usr/src/test
WORKDIR /usr/src/test
RUN apk add g++
RUN g++ -o STUN-test tests/test.cpp src/STUNResponse.cpp src/STUNResponseBuilder.cpp src/STUNOperations.cpp
CMD ["./STUN-test"]
