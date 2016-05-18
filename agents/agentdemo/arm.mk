CC = arm-unknown-linux-gnueabi-g++
SRCPATH = /home/horus/x-tools/arm-unknown-linux-gnueabi/arm-unknown-linux-gnueabi
CFLAGS = -g -I. -I../tinyxml -I../libs -I$(SRCPATH)/boost_1_53_0_arm/ -I$(SRCPATH)/include -I$(SRCPATH)/usr/lib/openssl-arm-unknown/include -I$(SRCPATH)/usr/lib/libmodbus/include/ -DNDEBUG
LIB = -lstdc++ -lpthread
BOOSTLIB = -L$(SRCPATH)/boost_1_53_0_arm/stage/lib -lboost_system -lboost_thread
SSLLIB = -L$(SRCPATH)/usr/lib/openssl-arm-unknown/lib -lssl -lcrypto 
MODBUS = $(SRCPATH)/usr/lib/libmodbus/lib/libmodbus.a

AGENTLIB = ../libs/*.o
OBJ = agent.o DummySource.o DummyPit.o
JSONLIB = ../libjson/Objects_static/*.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h DummySource.h
	$(CC) $(CFLAGS) -o soapagentx main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(BOOSTLIB) $(SSLLIB) $(MODBUS)

%.o:	%.cpp
	$(CC) -c $(CFLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp soapagent $(BIN)

clean:	
	rm *.o soapagentx
