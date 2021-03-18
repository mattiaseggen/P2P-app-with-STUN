SRC = ./src/

STUNServer: STUNServer.o STUNOperations.o
	g++ STUNServer.o STUNOperations.o -o STUNServer

STUNServer.o: STUNServer.cpp
	g++ -c STUNServer.cpp

STUNOperations.o: STUNOperations.cpp
	g++ -c STUNOperations.cpp


clean:
	rm *.o STUNServer
