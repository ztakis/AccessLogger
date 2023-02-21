// WiFi related functions

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include <Arduino.h>
#include "WiFiEsp.h"
#include "WiFiEspUdp.h"
// #include "WiFiEspAT.h"
#include "RTClib.h"
#include "arduino_secrets.h"

extern RTC_PCF8523 rtc;

extern WiFiEspUDP Udp;
// extern WiFiUDP Udp;

extern char ssid[];
extern char pass[];
extern int status;

extern const uint8_t NTP_PACKET_SIZE;
extern const uint16_t UDP_TIMEOUT;
extern byte packetBuffer[];

extern char timeServer[];
extern const uint16_t localPort;
extern const int GMT;

void wifiInit();
void printWifiStatus();
void rtcAdjustNTP();
void sendNTPpacket(char *ntpSrv);

#endif

