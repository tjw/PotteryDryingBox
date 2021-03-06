#include "Temperature.h"

#include "DHT.h"
#include "pins.h"

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(TemperatureSensorPin, DHTTYPE);

void Temperature::setup(void)
{
	dht.begin();
}

bool Temperature::read(Result &result)
{
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	result.temperature = dht.readTemperature();
	result.humidity = dht.readHumidity();
	
	// check if returns are valid, if they are NaN (not a number) then something went wrong!
	if (isnan(result.temperature) || isnan(result.humidity)) {
		Serial.println("Failed to read from DHT");
		return false;
	} else {
		// Serial.print("Humidity: "); 
		// Serial.print(h);
		// Serial.print(" %\t");
		// Serial.print("Temperature: "); 
		// Serial.print(t);
		// Serial.println(" *C");
		return true;
	}
}
