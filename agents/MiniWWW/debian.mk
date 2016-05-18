CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I../libjson -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -L../libjson -ljson -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
BIN = ../bin
OBJ = agent.o Configurator.o ConfRoutingMap.o

XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) $(OPTLIB) -o miniwww main.cpp $(OBJ) $(AGENTLIB) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o :	openssl/%.c
	$(CC) -c $<

install:	all
	cp miniwww $(BIN)/miniwww

clean:
	rm *.o miniwww

