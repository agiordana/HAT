CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/usr/local/include/boost -I../libjson -g
#LIB = -lstdc++ -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex
LIB = -L/usr/local/lib -lstdc++ -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lwebsocketpp -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex 
AGENTLIB = ../minilibs/*.o
OBJ = agent.o GPIOThread.o VirtualDevices.o
JSONLIB = ../libjson/Objects_static/*.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h GPIOThread.h VirtualDevices.h
	$(CC) $(FLAGS) -o bbdevices main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) 

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp bbdevices $(BIN)/bbdevices

clean:	
	rm *.o bbdevices
