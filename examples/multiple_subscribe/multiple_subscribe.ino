#include <ultimate_esp_mqtt_ubidots.h>
#include "user_macros.h"

esp_updater OTA_update;
esp_mqtt esp_ubidots(MQTT_CLIENT_NAME, TOKEN, DEBUG_FALSE);  
// esp_timer mq135_timer(5,'m',"mq135");       // User timer to see subscribed data

void setup()
{
    Serial.begin(115200);
    Serial.println("Waether data visualize");

    OTA_update.start_wifi_manager();
    Serial.print("Firmware Version:");
    Serial.println(FIRMWARE_VERSION);
    // Check Update
    Serial.println("Checking For Firmware Update....");

    OTA_update.start_http_update(OTA_URL, FIRMWARE_VERSION);

    // Start MQTT connection
    esp_ubidots.init();
    // If Mqtt subscribe, then subscribe
    esp_ubidots.mqtt_subscribe(POND_TEMPERATURE_TOPIC,HUMIDITY_TOPIC,TEMPERATURE_TOPIC, NULL); //Can enter Multiple topics, must be terminated with NULL
}

void loop()
{
    esp_ubidots.mqtt_loop();
    esp_ubidots.mqtt_subscribe(POND_TEMPERATURE_TOPIC,HUMIDITY_TOPIC,TEMPERATURE_TOPIC, NULL); //Can enter Multiple topics, must be terminated with NULL
}

void mqtt_user_code(char* mq_topic, char* payload, int len)
{
    //Stub function if user dont need to act on Subscribed data 

    //Payload analyzed to take action
    if(esp_ubidots.debug == DEBUG_TRUE)
    {
        Serial.print("Message arrived [");
        Serial.print(mq_topic);
        Serial.print("] ");

        Serial.println(payload);
    }
    float f_value = atof(payload);
    if(strcmp(mq_topic,POND_TEMPERATURE_TOPIC) == 0)
    {
        //If Pond temperature topic is received 
        Serial.print("Pond Temperature: ");
        Serial.println(f_value);
    }
    else if(strcmp(mq_topic,HUMIDITY_TOPIC) == 0)
    {
        //If Humodity Topic is recieved 
        Serial.print("Humidity: ");
        Serial.println(f_value);
    }
    else if(strcmp(mq_topic,TEMPERATURE_TOPIC) == 0)
    {
        //If temperature topic is recieved 
        Serial.print("Temperature: ");
        Serial.println(f_value);
    }
    
}