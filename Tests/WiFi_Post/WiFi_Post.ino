/*************************************************** 
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /*
This example does a test of the TCP client capability:
  * Initialization
  * Optional: SSID scan
  * AP connection
  * DHCP printout
  * DNS lookup
  * Optional: Ping
  * Connect to website and print out webpage contents
  * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// Used pins in CC3000 shield:
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// also used, SPI pins on UNO, 11-13

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER); // you can change this clock speed

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

/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Started!\n")); 

  /* Initialise the module */
  Serial.println(F("\nInitializing CC3000..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
//  while (! displayConnectionDetails()) {
//    delay(1000);
//  }

  ip = 0;
  // Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  // cc3000.printIPdotsRev(ip);
  
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  // Serial.println(F("\n\nDisconnecting"));
  // cc3000.disconnect();
  
postData(67.89, -43.25);
}

void loop(void)
{
	delay(1000);
	// int counter = 0;
	// 
	// while (1) {
	// 	postData(67.89, -43.21);
	// 	counter++;
	// 	delay(1000);
	// }
}

// Only handling numbers in -100..100 or so.
static void appendPlace(Adafruit_CC3000_Client &www, float &value, float power, bool &hasFirstDigit)
{
	int place = int(value/power);
	if (place == 0 && hasFirstDigit == false)
		return; // Drop leading zeros
	
	hasFirstDigit = true;
	
	char digit = '0' + place;
	value = value - place*power;
		
	char buf[2] = {digit, 0};
	www.fastrprint(buf);
}

// Using the Print class to convert float->string uses ~1k of memory in the program, but has more accurate output and is less code.
class WWWPrint : public Print {
	Adafruit_CC3000_Client &_www;
public:
	WWWPrint(Adafruit_CC3000_Client &www);

    size_t write(uint8_t);
};

WWWPrint::WWWPrint(Adafruit_CC3000_Client &www) : _www(www) {}
size_t WWWPrint::write(uint8_t c)
{
	char buf[2] = {c, 0};
	_www.fastrprint(buf);
	return 1;
}

// Include leading zeros in this version
static void appendPlace(Adafruit_CC3000_Client &www, float &value, float power)
{
	bool hasFirstDigit = true;
	appendPlace(www, value, power, hasFirstDigit);
}
static void appendFloat(Adafruit_CC3000_Client &www, float value)
{
	WWWPrint p(www);
	p.print(value);
	return;
	
	if (value < 0) {
		www.fastrprint("-");
		value = -value;
	}
	if (value > 100.0)
		value = 100.0;
	
	bool hasFirstDigit = false;
	appendPlace(www, value, 100.0, hasFirstDigit);
	appendPlace(www, value, 10.0, hasFirstDigit);
	appendPlace(www, value, 1.0, hasFirstDigit);
	if (!hasFirstDigit)
		www.fastrprint("0");
	www.fastrprint(".");
	appendPlace(www, value, 0.1);
	appendPlace(www, value, 0.01);
}

static void postData(float temperature, float humidity)
{
 /* Try connecting to the website.
    Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
 */
 Adafruit_CC3000_Client www = cc3000.connectTCP(ip, PORT);
 if (www.connected()) {
   www.fastrprint(F("POST "));
   www.fastrprint(WEBPAGE);
   
   www.fastrprint("?temperature="); appendFloat(www, temperature);
   www.fastrprint("&humidity="); appendFloat(www, humidity);
   
   www.fastrprint(F(" HTTP/1.1\r\n"));
   www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
   www.fastrprint(F("Content-Length: 0\r\n"));
   www.fastrprint(F("\r\n"));
   www.println();
 } else {
   Serial.println(F("Connection failed"));    
   return;
 }

 Serial.println(F("-------------------------------------"));
 
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
 Serial.println(F("-------------------------------------"));
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = cc3000.startSSIDscan();

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
/*
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
*/