// Can use PWM, but the fan I have makes a bit of a wierd noise at non-full power
// Pin 6 used since 3 is used by wi-fi shield for IRQ
int fan = 6; // PWM pin

void setup() {                
	pinMode(fan, OUTPUT);     
}

void loop() {
	// Turn it on for a while and then off
    analogWrite(fan, 0);
	delay(4000);

    analogWrite(fan, 255);
	delay(8000);
}
