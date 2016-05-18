CC = arm-unknown-linux-gnueabi-g++
SRCPATH = /home/horus/x-tools/arm-unknown-linux-gnueabi/arm-unknown-linux-gnueabi
CFLAGS = -g -I. -I../tinyxml -I../libs -I$(SRCPATH)/boost_1_53_0_arm/ -I$(SRCPATH)/include -I$(SRCPATH)/usr/lib/openssl-arm-unknown/include -I$(SRCPATH)/usr/lib/libmodbus/include/ -DNDEBUG
LIB = -lstdc++ -lpthread
BOOSTLIB = -L$(SRCPATH)/boost_1_53_0_arm/stage/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex
SSLLIB = -L$(SRCPATH)/usr/lib/openssl-arm-unknown/lib -lssl -lcrypto 
MODBUS = $(SRCPATH)/usr/lib/libmodbus/lib/libmodbus.a

AGENTLIB = ../libs/*.o
OBJ = agent.o ProgramEngine.o TimedRuleEngine.o eventcounter.o ruleset.o timedruleset.o RuleEngine.o ActionEngine.o actionset.o action.o eventcounterset.o rule.o timedrule.o Scheduler.o Control.o controlset.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(CFLAGS) -o mngagentx main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(SSLLIB) $(BOOSTLIB) $(MODBUS) 

%.o:	%.cpp
	$(CC) -c $(CFLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp mngagentx $(BIN)

clean:	
	rm *.o mngagentx
