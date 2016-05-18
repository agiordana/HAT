CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I../libjson -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -L../libjson -ljson -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
BIN = /home/horus/bin
OBJ = agent.o 

XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

all:	enableconf.cpp
	$(CC) $(FLAGS) $(OPTLIB) -o enableconf enableconf.cpp $(AGENTLIB) $(XML) $(LIB) $(JSON)

install:	all
	cp enableconf $(BIN)/enableconf

clean:
	rm *.o enableconf

