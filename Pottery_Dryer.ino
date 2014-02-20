#include "DHT.h"

#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Pins
#define TEMP_HUM (2)
#define FAN (3) // PWM for variable fan speed
#define HEATER (4)
#define PIXEL (6)

DHT dht(TEMP_HUM, DHTTYPE);

void setup() {                
	pinMode(FAN, OUTPUT);     
	pinMode(HEATER, OUTPUT);     
	pinMode(PIXEL, OUTPUT);     

    Serial.begin(9600); 
    Serial.println("DHT begin!");
    dht.begin();
}

void loop() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
	
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT");
    } else {
      Serial.print("Humidity: "); 
      Serial.print(humidity);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(temperature);
      Serial.println(" *C");
    }
	
	// Temperature sensor doesn't update very quickly, so chill.
	delay(2000);
}
