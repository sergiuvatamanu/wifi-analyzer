#pragma once
#include <string>

class NetworkListItem
{
public:
	NetworkListItem() {

	}

	std::string ssid;
	std::string mac;

	long rssi;
	unsigned long centerFrequency;
	int channel;

	std::string auth;
	std::string cipher;
};

