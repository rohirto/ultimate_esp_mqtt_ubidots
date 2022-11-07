#ifndef ULTIMATE_ESP_MQTT_UBIDOTS_H
#define ULTIMATE_ESP_MQTT_UBIDOTS_H
#include <Arduino.h>
#include <string.h>
#include <stdarg.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WebSocketsClient.h>
#include <DNSServer.h>
#include <PubSubClient.h>

#define MQTT_SERVER         "industrial.api.ubidots.com"

// Func Prototypes
void update_started();
void update_finished();
void update_progress(int, int);
void update_error(int);
void updater(WiFiClient, String, String);
void mqtt_user_code(char*, char*, int);

// MQTT func proto
void callback(char *, byte *, unsigned int); // MQTT callback func
// Classes
class esp_updater
{
private:
public:
    void start_wifi_manager();
    void start_http_update(WiFiClient, String, String);
};
class esp_mqtt
{
private:
    char *client_name;
    char *ubidots_token;
    char* server_name;
    char payload[700];
    char topic[150];

public:
    esp_mqtt(char *, char *, bool);
    void init();

    void reconnect(void); // reconnect to MQTT
    void mqtt_loop(void);
    void mqtt_subscribe(const char*,...);
    void publish_data(char*, char*, char*);
    bool connected;
    bool resubscribe;
    bool debug;
};
class esp_timer
{
private:
    char* timer_name;
    unsigned long startMillis; // Some global vaiable anywhere in program
    unsigned long currentMillis;
    volatile uint32_t timer_interval;

public:
    esp_timer(byte timer_interval, char , char*);
    volatile bool timer_elapsed;
    void timer_loop(void);
    void init(uint32_t);
    void start();
};
#endif