CC = g++
FLAGS = -I. -I../libs -I../tinyxml -I/usr/local/include -I../libjson -I/opt -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto 
OPTLIB = -L/usr/local/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../libs/*.o
OBJ = agent.o ProgramEngine.o TimedRuleEngine.o eventcounter.o ruleset.o timedruleset.o RuleEngine.o eventcounterset.o rule.o timedrule.o ActionEngine.o action.o actionset.o BooleanConditionSet.o BooleanCondition.o
JSONLIB = ../libjson/Objects_static/*.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) $(OPTLIB) -o mngagentx main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSONLIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp mngagentx $(BIN)

clean:	
	rm *.o mngagentx
