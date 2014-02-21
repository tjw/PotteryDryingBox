#include <Adafruit_CC3000/Adafruit_CC3000.h>

// Using the Print class to convert float->string uses ~1k of memory in the program, but has more accurate output and is less code.
class WWWPrint : public Print {
	Adafruit_CC3000_Client &_www;
public:
	WWWPrint(Adafruit_CC3000_Client &www);

	size_t write(uint8_t);
};
