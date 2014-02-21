// A list of all the pins on the Arduino we've used and why

enum {
	HeaterPin = 2, // Powerswitch Tail
	TemperatureSensorPin = 4, // DHT22
	FanPin = 6, // Transistor + 12v DC fan. Actually a PWM pin, but for now we'll run it on/off
	
	// Adafruit CC3000 -- these are hardwired on the shield
	WifiIRQPin = 3, // MUST be an interrupt pin!
	WifiVBATPin = 5, 
	WifiVCSPin = 10,
	WifiSPI0 = 11,
	WifiSPI1 = 12,
	WifiSPI2 = 13,
};
