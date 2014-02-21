#include "WWWPrint.h"

WWWPrint::WWWPrint(Adafruit_CC3000_Client &www) : _www(www) {}
size_t WWWPrint::write(uint8_t c)
{
	char buf[2] = {c, 0};
	_www.fastrprint(buf);
	return 1;
}
