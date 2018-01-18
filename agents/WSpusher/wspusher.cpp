#include "server_ws.hpp"
#include "client_ws.hpp"
#include "sstream"

using namespace std;

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;


int main(int argc, char *argv[]) {
    //WebSocket (WS)-server at port 8080 
    unsigned short port = 8080;
    if(argc >1) sscanf(argv[1],"%u",&port);

    WsServer server;
    server.config.port=port;
    

    auto& broadcast=server.endpoint["^/events/?$"];
        
    broadcast.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
       auto message_str=message->string();
       auto send_stream=make_shared<WsServer::SendStream>();
       auto send_stream_answer=make_shared<WsServer::SendStream>();
       *send_stream << message_str;
       for(auto a_connection: server.get_connections()) 
	  if(a_connection != connection) {


            //server.send is an asynchronous function
           a_connection->send(send_stream);
       }
       *send_stream_answer << "received";
       connection->send(send_stream_answer);
       
    };

    broadcast.on_open = [](shared_ptr<WsServer::Connection> connection) {
       cout << "Server: Opened connection " << connection.get() << endl;
    };

    
    server.start();

  
    return 0;
}
