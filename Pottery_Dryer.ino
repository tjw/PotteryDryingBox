// These have to be here for the Arduino IDE to automatically add -I directives to the compile...
#include "Adafruit_CC3000.h"
#include "DHT.h"
#include "SPI.h"

#include "xWiFi.h"
#include "Temperature.h"
#include "ToggleDevice.h"
#include "pins.h"

static ToggleDevice Fan(FanPin);
static ToggleDevice Heater(HeaterPin);
	
void setup(void)
{
	Serial.begin(115200);
	Serial.println("Pottery Dryer Started!\n"); 

	Temperature::setup();
	WiFi::setup();
	WiFi::postInfo("starting");

	Heater.setup();
	Fan.setup();

	// Start heating now and forever until we are done. It could be that we'll be able to turn off the heat for periods while the humidity builds so that we can dry more slowly.
	Heater.on();

	// Quick test that these outputs are working
	Fan.on();
	delay(2000);
	Fan.off();
}

/*
Heat to the target minimum temperature.
Once reached:
   - If the humidity has been high for a while, vent for a few seconds
   - If the temperature exceeds the maximum target temperature, vent for a few seconds?
*/
static const float TargetMinTemperature = 40.0; // 104 F
static const float TargetMaxTemperature = 42.0; // In full-on heating test, it took from 2014-02-20T16:07:18-08:00 to 2014-02-20T17:35:30-08:00 to go from 40.0 to 42.0.

static const float TargetHumidity = 99.0; // Want it very humid to keep the work's surface open to moisture flowing

static unsigned ConsecutiveAboveTemperatureReadings = 0;
static unsigned FanCycles = 0;
static bool Done = false;
static int NoDateErrorCount = 0;

void loop(void)
{
	if (Done)
		return;
	
	Temperature::Result result;
	if (!Temperature::read(result)) {
		WiFi::postInfo("no_data");
		NoDateErrorCount++;
		if (NoDateErrorCount > 5) {
			// Just turn off after a bit if something seems wrong
			WiFi::postInfo("done");
			Done = true;
			return;
		}
	} else {
		WiFi::postData(result.temperature, result.humidity, FanCycles);
	}
	
	if (result.temperature >= TargetMinTemperature)
		ConsecutiveAboveTemperatureReadings++;
	else
		ConsecutiveAboveTemperatureReadings = 0; // Fan was probably just on, or we just started up
	
	if (ConsecutiveAboveTemperatureReadings > 5 && result.humidity >= TargetHumidity) {
		// Run the fan for a little bit to eject some of the moisture. This should lower both the temperature and humidity.
		Fan.on();
		delay(10000);
		Fan.off();
		FanCycles++;
	}
	
	// Eventually, heating will not produce more moisture.
	if (ConsecutiveAboveTemperatureReadings > 100 && result.humidity < TargetHumidity) {
		Heater.off();
		WiFi::postInfo("done");
		Done = true;
		return;
	}
	
	delay(30000);
}
