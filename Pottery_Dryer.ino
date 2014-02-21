// These have to be here for the Arduino IDE to automatically add -I directives to the compile...
#include "Adafruit_CC3000.h"
#include "DHT.h"
#include "SPI.h"

#include "xWiFi.h"
#include "Temperature.h"

void setup(void)
{
	Serial.begin(115200);
	Serial.println("Pottery Dryer Started!\n"); 

	Temperature::setup();
	WiFi::setup();
}

void loop(void)
{
	Temperature::Result result;
	if (!Temperature::read(result)) {
		WiFi::postData(-1, -1);
	} else {
		WiFi::postData(result.temperature, result.humidity);
	}
	delay(30000);
}
