
class ToggleDevice {
private:
	int _pin;
public:
	ToggleDevice(int pin) : _pin(pin) {}
	
	void setup(void) {
		pinMode(_pin, OUTPUT);
	}
	void on(void) {
		digitalWrite(_pin, HIGH);
	}
	void off(void) {
		digitalWrite(_pin, LOW);
	}
};

