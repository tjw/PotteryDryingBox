#define POWER_SWITCH (2)

void setup() {
	pinMode(POWER_SWITCH, OUTPUT);     
}

void loop() {
	digitalWrite(POWER_SWITCH, HIGH);
	// delay(5000);
	// digitalWrite(POWER_SWITCH, LOW);
	// delay(5000);
}