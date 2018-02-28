#ifndef TEMPERATURE_CONVERSION_H
#define TEMPERATURE_CONVERSION_H

#include <mosquittopp.h>
#include <string>
#include <vector>

class MqttClient : public mosqpp::mosquittopp {
public:
	MqttClient(const char *id, const char *host, int port, std::vector<std::string>& subjects);
	~MqttClient();

	void on_connect(int rc);
	void on_message(const struct mosquitto_message *message);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);
	bool publish(std::string& topic, std::string& message);
private:
	std::vector<std::string> tosubscribe;
};

#endif
