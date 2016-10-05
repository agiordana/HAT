CCPP = g++
CC = gcc
FLAGS = -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG 
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../minilibs/*.o

BIN = ../bin
XML = ../tinyxml/*.o

all:	websocket pusher

websocket:	websocket.cpp 
	$(CCPP) $(FLAGS) $(OPTLIB) -o websocket websocket.cpp $(AGENTLIB) $(XML) $(LIB)

pusher:	pusher.c 
	$(CC) $(OPTLIB) -o pusher pusher.c 

install:	all
	cp pusher $(BIN)/pusher
	cp websocket $(BIN)/websocket
	cp websocketd $(BIN)/websocketd
	chmod a+x $(BIN)/pusher
	chmod a+x $(BIN)/websocket
	chmod a+x $(BIN)/websocketd

clean:	
	rm websocket pusher
