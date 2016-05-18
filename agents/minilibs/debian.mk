CC = g++
CFLAGS = -g -I. -I../tinyxml -I/opt/include -I/usr/local/include -I/usr/local/include/httpserver -DNDEBUG 
AGENTLIB = agentlib.o attribute.o Mattribute.o namelist.o filemanager.o httpserver.o logger.o net.o phone.o remoteagent.o period.o dayprogram.o program.o alarm.o  hsrv.o iplocation.o message.o phone.o pwrprogram.o  MMessage.o MRemoteAgent.o Trigger.o agentnet.o udpserver.o udpclient.o cipher.o Observer.o Subject.o SubjectSet.o ObserverTCP.o ObserverThread.o ArchiveManager.o RestHttpServer.o Route.o ArchiveRoute.o CmdRoute.o LoggerRoute.o RpcCall.o NameService.o RpcWaitTable.o

XML = ../tinyxml

all:	$(AGENTLIB)

%.o:	%.cpp %.h hsrv.h agentlib.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o
