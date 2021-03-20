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


make test: test.o STUNOperations.o STUNResponse.o STUNResponseBuilder.o
	g++ test.o STUNOperations.o STUNResponse.o STUNResponseBuilder.o -o TestProgram

test.o: $(TEST)test.cpp
	g++ -c $(TEST)test.cpp


clean:
	rm *.o STUNServer TestProgram