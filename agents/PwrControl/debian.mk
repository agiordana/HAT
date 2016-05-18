CC = g++
FLAGS = -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG  
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../minilibs/*.o
OBJ = agent.o Controller.o Scheduler.o Control.o ControlSet.o Sensor.o SensorSet.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) $(OPTLIB) -o pwrcontrol main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp pwrcontrol $(BIN)/pwrcontrol

clean:	
	rm *.o pwrcontrol
