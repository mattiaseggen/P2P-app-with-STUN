SRC = ./src/
TEST = ./test/

STUNServer: STUNServer.o STUNOperations.o STUNResponse.o STUNResponseBuilder.o
	g++ STUNServer.o STUNOperations.o STUNResponse.o STUNResponseBuilder.o -lpthread -o STUNServer

STUNServer.o: $(SRC)STUNServer.cpp
	g++ -c $(SRC)STUNServer.cpp

STUNOperations.o: $(SRC)STUNOperations.cpp
	g++ -c $(SRC)STUNOperations.cpp

STUNResponse.o: $(SRC)STUNResponse.cpp
	g++ -c $(SRC)STUNResponse.cpp

STUNResponseBuilder.o: $(SRC)STUNResponseBuilder.cpp
	g++ -c $(SRC)STUNResponseBuilder.cpp

test: 
	docker build -t stun-test -f tests/Dockerfile .
	docker run --rm stun-test

clean:
	rm *.o STUNServer STUN-test

run:
	docker-compose build stun-server
	docker-compose up
