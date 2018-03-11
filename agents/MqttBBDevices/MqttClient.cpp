#include <cstdio>
#include <cstring>

#include "MqttClient.h"
#include <mosquittopp.h>
#define MAXBUF 16000

using namespace std;

MqttClient::MqttClient(const char *id, const char *host, int port, vector<string>& subjects) : mosquittopp(id) {
	int keepalive = 600;

	/* Connect immediately. This could also be done by calling
	 * MqttClient->connect(). */
	for(size_t i=0; i<subjects.size(); i++) 
	    if(hsrv::mqtt_domain == "") tosubscribe.push_back(subjects[i]);
		else tosubscribe.push_back(hsrv::mqtt_domain+"/"+subjects[i]);
	connect(host, port, keepalive);
};

MqttClient::~MqttClient() { }

void MqttClient::on_connect(int rc) {
//	printf("Connected with code %d.\n", rc);
        if(rc == 0) 
	   for(size_t i=0; i<tosubscribe.size(); i++) {
	       subscribe(NULL, tosubscribe[i].c_str());
	   }
}

void MqttClient::on_message(const struct mosquitto_message *message) {

//	printf("topic: %s\n", message->topic);
//	printf("%s\n", message->payload);	
   char buf[MAXBUF];
   sprintf(buf,"%s", message->payload);
   string topic = message->topic;
   string mm = buf;
   sendCmd(topic, mm);
}

void MqttClient::on_subscribe(int mid, int qos_count, const int *granted_qos) {
//	printf("Subscription succeeded.\n");
}

bool MqttClient::publish(string& topic, string& message) {
   int res;
   int ln = (int) message.size();
   mosqpp::mosquittopp::publish(NULL, topic.c_str(), ln, message.c_str());
   res = loop();
   while(res) reconnect();
//   printf("res %d\n", res);
   if(res == 0) return true;
   else return false;
}

bool MqttClient::sendCmd(string topic, string message) {
   MMessage msg;
   NameList names;
   names.init(topic,'/');
   size_t cmd_index = 0;
   if(names.size()<3) return false;
   if(names[cmd_index] == hsrv::mqtt_domain) cmd_index++;
   if(names[cmd_index] == "wget") {
      NameList path;
      path.init(message, '/');
      if(path[0]=="DynamicPage" || path[0]=="DynamicPage.xml"|| path[0]=="DynamicPage.json") {
	string response = hsrv::getDynamicPage(message);
	string pub_topic;
	if(hsrv::mqtt_domain!="") pub_topic = hsrv::mqtt_domain+"/wgetr";
	  else pub_topic = "wgetr";
	if(isTag(names[cmd_index+1])) {
	   pub_topic += "/";
	   pub_topic += names[cmd_index+1];
	}
        MMessage to_pub("mqtt", "mqtt_send");
 	to_pub.add("topic", pub_topic);
        to_pub.add("value", response);
	SubjectSet::notify(to_pub);
      }
   }
   else {
     msg.mtype = names[cmd_index];
     msg.msubtype = names[cmd_index+1];
     if(names.size()>cmd_index+2 && isTag(names[cmd_index+2])) msg.add("mqtttag", names[cmd_index+2]);
     NameList values;
     values.init(message, '=');
     if(values.size()<2) return false;
     msg.add(values[0], values[1]);
     SubjectSet::notify(msg);
     return true;
   }
}

bool MqttClient::isTag(string& name) {
  if(name.length() >2 && name[0] == 't' && name[1] == '.') return true;
  else return false;
}
