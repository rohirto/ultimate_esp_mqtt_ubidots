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
#include <NTPClient.h>
#include <WiFiUdp.h>

#define MQTT_SERVER         "industrial.api.ubidots.com"

//GPIOS def for Generic ESP8266
#define GPIO_5                      5
#define GPIO_4                      4
#define GPIO_0                      0
#define GPIO_2                      2
#define GPIO_15                     15
#define GPIO_16                     16
#define GPIO_14                     14
#define GPIO_12                     12
#define GPIO_13                     13

// Func Prototypes
void update_started();
void update_finished();
void update_progress(int, int);
void update_error(int);
void updater(String, String);
void mqtt_user_code(char*, byte*, int);

// MQTT func proto
void callback(char *, byte *, unsigned int); // MQTT callback func
// Classes
class esp_updater
{
private:
public:
    void start_wifi_manager();
    void wifi_manager_reset();
    void start_http_update(String, String);
};
class esp_mqtt
{
private:
    const char *client_name;
    const char *ubidots_token;
    const char* server_name;
    char payload[700];
    char topic[150];

public:
    esp_mqtt(const char *,const char *, bool);
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
    const char* timer_name;
    unsigned long startMillis; // Some global vaiable anywhere in program
    unsigned long currentMillis;
    volatile uint32_t timer_interval;

public:
    esp_timer(byte timer_interval,const char , const char*);
    volatile bool timer_elapsed;
    void timer_loop(void);
    void init(uint32_t);
    void start();
};

class esp_ntp
{
    private:
        const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        
        
    public:
        esp_ntp();
        volatile int current_hr;
        volatile int current_min;
        void get_time_date(void);
        const long utcOffsetInSeconds = 19800;  //IST time offset

};
#endif