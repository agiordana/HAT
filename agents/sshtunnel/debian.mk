CCPP = g++
CC = gcc
FLAGS = -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG 
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../minilibs/*.o

BIN = ../bin
XML = ../tinyxml/*.o

all:	sshtunnel

sshtunnel:	sshtunnel.cpp 
	$(CCPP) $(FLAGS) $(OPTLIB) -o sshtunnel sshtunnel.cpp $(AGENTLIB) $(XML) $(LIB)

install:	all
	cp sshtunnel $(BIN)/sshtunnel
	chmod a+x $(BIN)/sshtunnel

clean:	
	rm sshtunnel
