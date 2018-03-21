// This demo does web requests to a fixed IP address, using a fixed gateway.
// 2010-11-27 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x32 };
// ethernet interface ip address
static byte myip[] = { 192,168,1,100 };
// gateway ip address
static byte gwip[] = { 192,168,1,254 };
// remote website ip address and port
//static byte hisip[] = { 74,125,79,99 };
// remote website name
const char website[] PROGMEM = "google.com";

byte Ethernet::buffer[300];   // a very small tcp/ip buffer is enough here
static long timer;
boolean ledStatus;
const int ledPin = 13;
char* on = "ON";
char* off = "OFF";
char* statusLabel;
char* buttonLabel;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[getStaticIP]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  ether.staticSetup(myip, gwip);

  //ether.copyIp(ether.hisip, hisip);
  //ether.printIp("Server: ", ether.hisip);

  while (ether.clientWaitingGw())
    ether.packetLoop(ether.packetReceive());
  Serial.println("Gateway found");
  
  timer = - REQUEST_RATE; // start timing out right away

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip); 

}

void loop () {
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if(pos) {
    
    if(strstr((char *)Ethernet::buffer + pos, "GET /?alarm=ON") != 0) {
      Serial.println("Received ON command");
      ledStatus = true;
    }

    if(strstr((char *)Ethernet::buffer + pos, "GET /?alarm=OFF") != 0) {
      Serial.println("Received OFF command");
      ledStatus = false;
    }
    
    if(ledStatus) {
      digitalWrite(ledPin, HIGH);
      statusLabel = on;
      buttonLabel = off;
    } else {
      digitalWrite(ledPin, LOW);
      statusLabel = off;
      buttonLabel = on;
    }
      
    BufferFiller bfill = ether.tcpOffset();
    bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\nPragma: no-cache\r\n\r\n"
      "<html><head><title>WebLed</title></head>"
      "<body>LED Alarm: $S "
      "<a href=\"/?alarm=$S\"><input type=\"button\" value=\"$S\"></a>"
      "</body></html>"      
      ), statusLabel, buttonLabel, buttonLabel);
    ether.httpServerReply(bfill.position());
  }
}
