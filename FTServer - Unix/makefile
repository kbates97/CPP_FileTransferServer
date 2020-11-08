all: directories FTServer FTClient

directories:
	mkdir bin lib

FTServer: FTServe.o FTServerProgram.o FTConnectedClient.o libPRSLib.a
	g++ -o FTServer bin/FTServe.o bin/FTServerProgram.o bin/FTConnectedClient.o -Llib -lPRSLib

FTClient: FTClientProgram.o FTClient.o libPRSLib.a
	g++ -o FTClient bin/FTClientProgram.o bin/FTClient.o -Llib -lPRSLib

FTServe.o: FTServe.cpp FTServe.hpp
	g++ -c -o bin/$@ FTServe.cpp

FTServerProgram.o: FTServerProgram.cpp FTServe.hpp PRSClient.hpp
	g++ -c -o bin/$@ FTServerProgram.cpp

FTConnectedClient.o: FTConnectedClient.cpp FTConnectedClient.hpp
	g++ -c -o bin/$@ FTConnectedClient.cpp

FTClientProgram.o: FTClientProgram.cpp FTClient.hpp PRSClient.hpp
	g++ -c -o bin/$@ FTClientProgram.cpp

FTClient.o: FTClient.cpp FTClient.hpp
	g++ -c -o bin/$@ FTClient.cpp

libPRSLib.a: PRSMessage.o PRSClient.o
	ar -rcs lib/libPRSLib.a bin/PRSMessage.o bin/PRSClient.o

PRSMessage.o: PRSMessage.cpp
	g++ -c -o bin/$@ PRSMessage.cpp

PRSClient.o: PRSClient.cpp
	g++ -c -o bin/$@ PRSClient.cpp

clean:
	rm -r bin lib FTServer FTClient

