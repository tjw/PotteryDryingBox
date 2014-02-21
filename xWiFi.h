#include "Adafruit_CC3000.h"

class WiFi {
public:
	static void setup(void);
	static void postData(float temperature, float humidity);
};
