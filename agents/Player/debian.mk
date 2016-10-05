CC = g++
FLAGS = -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG 
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../minilibs/*.o
OBJ = agent.o eventcounter.o ruleset.o RuleEngine.o eventcounterset.o rule.o ActionEngine.o action.o actionset.o BooleanConditionSet.o BooleanCondition.o Configurator.o ConfRoutingMap.o

BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) $(OPTLIB) -o player main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp player $(BIN)/player

clean:	
	rm *.o player
