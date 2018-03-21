// Demo using DHCP and DNS to perform a web client request.
// 2011-06-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
static uint32_t timer;
const int buttonPin = 2; 

const char website[] PROGMEM = "www.google.com";

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[Client Allarmatore]"));
  pinMode(buttonPin, INPUT_PULLUP);

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  



  // if website is a string containing an IP address instead of a domain name,
  // then use it directly. Note: the string can not be in PROGMEM.
  /*char websiteIP[] = "192.168.1.100";
  ether.parseIp(ether.hisip, websiteIP);*/

  // or provide a numeric IP address instead of a string
  byte hisip[] = { 192,168,1,100 };
  ether.copyIp(ether.hisip, hisip);

    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer) {
    if (digitalRead(buttonPin) == LOW) {
      timer = millis() + 2000;
      Serial.println();
      Serial.print("<<< REQ ");
      ether.browseUrl(PSTR("/"), "?alarm=ON", website, my_callback);
    }
  }
}
