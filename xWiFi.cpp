#include "Arduino.h"
#include "xWiFi.h"
#include "pins.h"

#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "WString.h"

#include "Adafruit_CC3000.h"
#include "utility/debug.h"


#define WLAN_SSID       "wood"
#define WLAN_PASS       "oj9H3wgveJf7y"
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
// received before closing the connection.  If you know the server
// you're accessing is quick to respond, you can reduce this value.

// The CC3000 library only seems to support normal DNS, not mDNS, though there is possibly support for that at http://forum.pjrc.com/threads/24481-ZeroConf-Bonjour-Library
#define WEBSITE      "192.168.0.104"
#define PORT         9123
#define WEBPAGE      "/"

uint32_t ip;

Adafruit_CC3000 cc3000 = Adafruit_CC3000(WifiVCSPin, WifiIRQPin, WifiVBATPin, SPI_CLOCK_DIVIDER); // you can change this clock speed

void WiFi::setup(void)
{
	/* Initialise the module */
	Serial.println("\nInitializing CC3000...");
	if (!cc3000.begin()) {
		Serial.println("Couldn't begin()! Check your wiring?");
		while(1);
	}
  
	if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
		Serial.println("Failed!");
		while(1);
	}
   
	Serial.println("Connected!");
  
	/* Wait for DHCP to complete */
	Serial.println("Request DHCP");
	while (!cc3000.checkDHCP())	{
		delay(100); // ToDo: Insert a DHCP timeout!
	}  

	/* Display the IP address DNS, Gateway, etc. */  
	//  while (! displayConnectionDetails()) {
	//    delay(1000);
	//  }

	ip = 0;
	// Try looking up the website's IP address
	Serial.print(WEBSITE); Serial.print(" -> ");
	while (ip == 0) {
		if (! cc3000.getHostByName((char *)WEBSITE, &ip)) {
			Serial.println("Couldn't resolve!");
		}
		delay(500);
	}

	// cc3000.printIPdotsRev(ip);
  
	/* You need to make sure to clean up after yourself or the CC3000 can freak out */
	/* the next time your try to connect ... */
	// Serial.println("\n\nDisconnecting");
	// cc3000.disconnect();
}

void WiFi::_readResponse(Adafruit_CC3000_Client &www)
{
	Serial.println("-------------------------------------");
 
	/* Read data until either the connection is closed, or the idle timeout is reached. */ 
	unsigned long lastRead = millis();
	while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
		while (www.available()) {
			char c = www.read();
			Serial.print(c);
			lastRead = millis();
		}
	}
	www.close();
	Serial.println("-------------------------------------");
}

static unsigned postCount = 0;
void WiFi::postData(float temperature, float humidity, uint32_t fanCycles)
{
	/* Try connecting to the website.
	Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
	*/
	Adafruit_CC3000_Client www = cc3000.connectTCP(ip, PORT);
	if (www.connected()) {
		www.fastrprint("POST ");
		www.fastrprint(WEBPAGE);
      
		www.fastrprint("?id=pottery_box=");
		www.fastrprint("&count="); www.print(postCount);
		www.fastrprint("&temperature="); www.print(temperature);
		www.fastrprint("&humidity="); www.print(humidity);
		www.fastrprint("&fanCycles="); www.print(fanCycles);
   
		www.fastrprint(" HTTP/1.1\r\n");
		www.fastrprint("Host: "); www.fastrprint(WEBSITE); www.fastrprint("\r\n");
		www.fastrprint("Content-Length: 0\r\n");
		www.fastrprint("\r\n");
		www.println();
   
		postCount++;
	} else {
		Serial.println("Connection failed");
		return;
	}

	_readResponse(www);
}

void WiFi::postInfo(const char *info)
{
	Adafruit_CC3000_Client www = cc3000.connectTCP(ip, PORT);
	if (www.connected()) {
		www.fastrprint("POST ");
		www.fastrprint(WEBPAGE);
      
		www.fastrprint("?id=pottery_box=");
		www.fastrprint("&info="); www.fastrprint(info);
   
		www.fastrprint(" HTTP/1.1\r\n");
		www.fastrprint("Host: "); www.fastrprint(WEBSITE); www.fastrprint("\r\n");
		www.fastrprint("Content-Length: 0\r\n");
		www.fastrprint("\r\n");
		www.println();
	} else {
		Serial.println("Connection failed");
		return;
	}

	_readResponse(www);
}

void WiFi::postSetting(const char *name, float value)
{
	Adafruit_CC3000_Client www = cc3000.connectTCP(ip, PORT);
	if (www.connected()) {
		www.fastrprint("POST ");
		www.fastrprint(WEBPAGE);
      
		www.fastrprint("?id=pottery_box=");
		www.fastrprint("&setting="); www.fastrprint(name);
		www.fastrprint("&value="); www.print(value);
   
		www.fastrprint(" HTTP/1.1\r\n");
		www.fastrprint("Host: "); www.fastrprint(WEBSITE); www.fastrprint("\r\n");
		www.fastrprint("Content-Length: 0\r\n");
		www.fastrprint("\r\n");
		www.println();
	} else {
		Serial.println("Connection failed");
		return;
	}

	_readResponse(www);
}
