// A list of all the pins on the Arduino we've used and why

enum {
	// DHT22
	TemperatureSensorPin = 4,
	
	// Adafruit CC3000 -- these are hardwired on the shield
	WifiIRQPin = 3, // MUST be an interrupt pin!
	WifiVBATPin = 5, 
	WifiVCSPin = 10,
	WifiSPI0 = 11,
	WifiSPI1 = 12,
	WifiSPI2 = 13,
};
