CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I../libjson -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -L../libjson -ljson -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
BIN = ../bin
OBJ = agent.o ConfRoutingMap.o Device.o Component.o Automaton.o Configurator.o

XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

netdevices:	$(OBJ) main.cpp agent.h Device.h Component.h Automaton.h ConfRoutingMap.h
	$(CC) $(FLAGS) $(OPTLIB) -o netdevices main.cpp $(OBJ) $(AGENTLIB) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o :	openssl/%.c
	$(CC) -c $<

install:	netdevices
	cp netdevices $(BIN)/netdevices

clean:
	rm -f *.o netdevices

