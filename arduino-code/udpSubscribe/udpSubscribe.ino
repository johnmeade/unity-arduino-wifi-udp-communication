/*

This sketch uses UDP over Ethernet to...

Register with an external IP (send message to a specified
IP and port until it is acknowledged)

Listen for instructions (UDP packets)

*/

#include <SPI.h>
//#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <signal.h> // this is for a Galileo fix, see setup() line 1

byte mac[] = { 0xB0, 0xE2, 0x65, 0x31, 0x9D, 0xE0 };
IPAddress ip(10, 1, 16, 137); // ip for this device
unsigned int listenPort = 8888;
unsigned int sendPort = 8888;
char unityIP[] = "10.1.16.129";

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char RegisterBuffer[] = "REGISTER_POET";

//EthernetUDP Udp;
WiFiUDP Udp;
int status = WL_IDLE_STATUS;
char ssid[] = "Felt Lab";
char pass[] = "f3lt12E4";
int keyIndex = 0;  

bool registered = false;

void setup() {
  // Galileo networking libraries monkeypatch
  signal(SIGPIPE, SIG_IGN);
  
  // open serial for debug
  Serial.begin(9600);
  Serial.println("Starting setup"); 
  delay(2000);
  
  // set up LED
  pinMode(13, OUTPUT); // on-board led
  digitalWrite(13, LOW);
  
//  Ethernet.begin(mac,ip);
  // check for the presence of the shield:
  Serial.println("Checking for WiFi shield"); 
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } else {
    Serial.println("Found WiFi shield"); 
  }
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid,pass);
  
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
  
  Udp.begin(listenPort);
}

void readPacket() {
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    memset(packetBuffer, 0, sizeof(packetBuffer));
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3) { Serial.print("."); }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    
    // handle
    char reply[4];
    // outgoing
    String ok = "OK";
    String err = "ERR";
    // incoming
    char reg_ok[] = "REGISTER_OK";
    char flash[] = "FLASH_LED";
    // route message
    if (strcmp(packetBuffer, reg_ok) == 0) {
      ok.toCharArray(reply, 4);
      registered = true;
    } else if (strcmp(packetBuffer, flash) == 0) {
      //TODO
      digitalWrite(13, HIGH);
      ok.toCharArray(reply, 4);
    } else {
      err.toCharArray(reply, 4);
    }
    
    // send the reply
    Serial.print("writing to packet: ");
    Serial.println(reply);
    Udp.beginPacket(unityIP, sendPort);
    Udp.write(reply);
    Udp.endPacket();
  }
}

void loop() {
  if (!registered) {
    delay(100);
    Udp.beginPacket(unityIP, sendPort);
    Udp.write(RegisterBuffer);
    Udp.endPacket();
  }
  delay(10);
  readPacket();
}


