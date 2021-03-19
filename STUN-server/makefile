SRC = ./src/
TEST = ./test/

STUNServer: STUNServer.o STUNOperations.o
	g++ STUNServer.o STUNOperations.o -o STUNServer

STUNServer.o: $(SRC)STUNServer.cpp
	g++ -c $(SRC)STUNServer.cpp

STUNOperations.o: $(SRC)STUNOperations.cpp
	g++ -c $(SRC)STUNOperations.cpp



make test: test.o
	g++ test.o -o TestProgram

test.o: $(TEST)test.cpp
	g++ -c $(TEST)test.cpp


clean:
	rm *.o STUNServer TestProgram