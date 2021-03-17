SRC = ./src/

STUNServer: STUNServer.o STUNOperations.o
	g++ STUNServer.o STUNOperations.o -o STUNServer

STUNServer.o: $(SRC)STUNServer.cpp
	g++ -c $(SRC)STUNServer.cpp

STUNOperations.o: $(SRC)STUNOperations.cpp
	g++ -c $(SRC)STUNOperations.cpp


clean:
	rm *.o STUNServer