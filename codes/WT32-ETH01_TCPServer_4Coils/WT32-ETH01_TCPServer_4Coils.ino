/*
  by Yaser Ali Husen
  ModbusTCP Server with 4 coils
  Using WT32-ETH01 Board

  Library modbus: ArduinoModbus
  Library for ethernet connection: https://github.com/khoih-prog/WebServer_WT32_ETH01
  Use Arduino Library Manager Installation:
      The best and easiest way is to use Arduino Library Manager. Search for WebServer_WT32_ETH01, 
      then select / install the latest version 
*/

#include <SPI.h>
//#include <Ethernet.h>  // Ethernet library v2 is required
#include <ArduinoRS485.h>  // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_ 3
#include <WebServer_WT32_ETH01.h>

// Select the IP address according to your local network
IPAddress myIP(192, 168, 0, 212);
IPAddress myGW(192, 168, 0, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

WiFiServer ethServer(502);
ModbusTCPServer modbusTCPServer;

int relay1 = 2;
int relay2 = 4;
int relay3 = 12;
int relay4 = 14;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //Output configuration
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  //Turn OFF all relays
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  // To be called before ETH.begin()
  WT32_ETH01_onEvent();
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  ETH.config(myIP, myGW, mySN, myDNS);

  WT32_ETH01_waitForConnect();

  // start the server
  ethServer.begin();

  // start the Modbus TCP server
  if (!modbusTCPServer.begin()) {
    Serial.println("Failed to start Modbus TCP Server!");
    while (1);
  }
  // configure 4 coils at address 0x00
  modbusTCPServer.configureCoils(0, 4);
}

void loop() {
  delay(50);
        // listen for incoming clients
        WiFiClient client = ethServer.available();
  if (client) {
  // a new client connected
  Serial.println("new client");
    // let the Modbus TCP accept the connection
    modbusTCPServer.accept(client);
    while (client.connected()) {
      // poll for Modbus TCP requests, while client connected
      modbusTCPServer.poll();
      // update relays
      updateRelay();
    }
    Serial.println("client disconnected");
  }
}

void updateRelay() {
  // read the current value of the coil
  int coilValue1 = modbusTCPServer.coilRead(0);
  int coilValue2 = modbusTCPServer.coilRead(1);
  int coilValue3 = modbusTCPServer.coilRead(2);
  int coilValue4 = modbusTCPServer.coilRead(3);

  digitalWrite(relay1, !coilValue1);
  delay(10);
  digitalWrite(relay2, !coilValue2);
  delay(10);
  digitalWrite(relay3, !coilValue3);
  delay(10);
  digitalWrite(relay4, !coilValue4);
}
