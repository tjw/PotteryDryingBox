#include "Adafruit_CC3000.h"

class WiFi {
private:
	static void _readResponse(Adafruit_CC3000_Client &www);
public:
	static void setup(void);
	static void postData(float temperature, float humidity, uint32_t fanCycles);
	static void postInfo(const char *info);
	static void postSetting(const char *name, float value);
};
