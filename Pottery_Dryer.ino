// These have to be here for the Arduino IDE to automatically add -I directives to the compile...
#include "Adafruit_CC3000.h"
#include "DHT.h"
#include "SPI.h"

#include "xWiFi.h"
#include "Temperature.h"
#include "ToggleDevice.h"
#include "pins.h"

/*

 TODO:
 - Maybe add an initial soak period once reaching the target temperature range (to allow the heat to penetrate the clay)... heading period might be long enough for this to happen naturally.
 - Actually obey the min/max heat and turn the heater off while above the minimum... will have to see if this is worth done once I see the temp/humidity drop from a fan cycle.

*/

// NOTE: In full-on heating test with empty box, covered up with towels, it took from 2014-02-20T16:07:18-08:00 to 2014-02-20T17:35:30-08:00 to go from 40.0 to 42.0.
class Targets {
private:
	float _temperature;
	float _humidity;
	
public:
	Targets(float t, float h) : _temperature(t), _humidity(h) {}
	float temperature(void) const { return _temperature; } 
	float humidity(void) const { return _humidity; } 
};

// Uncomment one of these sections. The exact needed target seems to depend on the amount of clay in the box too.
// Case: A small slab with a target of 40º/75% never reached 75% humdity and came out leather hard
// Case: A medium bowl (~1.5kg) and small solid pyramid (~250g) came out nearly dry.
// Case: A large slab built object kept the humidity at 99.9% for a long time and then stopped getting readings from the temp/humidity. Unclear if the connector was bad or if the sensor hates being that humid for a long time. Redid the connector (which had a loose-ish wire) and dried out the box a bit).
// TODO: Hook up a switch of some sort or programmable level selector

//Targets Target(40.0, 75.0);
//Targets Target(38.0, 65.0); // Still too high for a small slab
//Targets Target(35.0, 55.0); // Pretty dry, but still not bone dry
//Targets Target(35.0, 30.0); // Got bone dry on a small slab

// Trying large slab built container
Targets Target(35.0, 70.0);

static unsigned ConsecutiveAboveTemperatureReadings = 0;
static unsigned FanCycles = 0;
static bool Done = false;
static int NoDataErrorCount = 0;

static ToggleDevice Fan(FanPin);
static ToggleDevice Heater(HeaterPin);
	
void setup(void)
{
	Serial.begin(115200);
	Serial.println("Pottery Dryer Started!\n"); 

	Temperature::setup();
	WiFi::setup();
	WiFi::postInfo("starting");
	WiFi::postSetting("target_temperature", Target.temperature());
	WiFi::postSetting("target_humidity", Target.humidity());

	Heater.setup();
	Fan.setup();

	// Start heating now and forever until we are done. It could be that we'll be able to turn off the heat for periods while the humidity builds so that we can dry more slowly.
	Heater.on();

	// Quick test that these outputs are working
	Fan.on();
	delay(500);
	Fan.off();
}

void loop(void)
{
	if (Done) {
		Heater.off();
		Fan.off();
		return;
	}
	
	Temperature::Result result;
	if (!Temperature::read(result)) {
		WiFi::postInfo("no_data");
		NoDataErrorCount++;
		if (NoDataErrorCount >= 50) {
			// Just turn off after a bit if something seems wrong
			WiFi::postInfo("done");
			Done = true;
			return;
		}
	} else {
		WiFi::postData(result.temperature, result.humidity, FanCycles);
	}
	
	if (result.temperature >= Target.temperature())
		ConsecutiveAboveTemperatureReadings++;
	else
		ConsecutiveAboveTemperatureReadings = 0; // Fan was probably just on, or we just started up
	
	if (ConsecutiveAboveTemperatureReadings > 5 && result.humidity >= Target.humidity()) {
		// Run the fan for a little bit to eject some of the moisture. This should lower both the temperature and humidity.
		Fan.on();
		delay(10000);
		Fan.off();
		FanCycles++;
		
		// TODO: Try this in the next test:
		// Give the temperature sensor more time to adjust
		// ConsecutiveAboveTemperatureReadings = 0;
	}
	
	// Eventually, heating will not produce more moisture.
	if (ConsecutiveAboveTemperatureReadings > 100 && result.humidity < Target.humidity()) {
		WiFi::postInfo("done");
		Done = true;
		return;
	}
	
	delay(30000);
}
