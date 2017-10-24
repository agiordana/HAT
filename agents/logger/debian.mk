CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = agent.o Receiver.o ArchiveThread.o PushThread.o PushConnection.o RemoteServerThread.o
BIN = ../bin
XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

logger:	$(OBJ) main.cpp agent.h Receiver.h ArchiveThread.h PushThread.h RemoteServerThread.h PushConnection.h
	$(CC) $(FLAGS) $(OPTLIB) -o logger main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	logger
	cp logger $(BIN)/logger

clean:	
	rm *.o logger
