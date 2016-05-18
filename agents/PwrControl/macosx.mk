CC = g++
FLAGS = -I. -I../minilibs -I../tinyxml -I/usr/local/include/boost -I../libjson -g -DNDEBUG
LIB = -lstdc++ -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex
AGENTLIB = ../minilibs/*.o
OBJ = agent.o Controller.o Control.o ControlSet.o
PKG_CONFIG_PATH := /usr/local/lib/pkgconfig:/opt/local/lib/pkgconfig
JSONLIB = ../libjson/Objects_static/*.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) -o pwrcontrol main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSONLIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp pwrcontrol $(BIN)/pwrcontrol

clean:	
	rm *.o pwrcontrol
