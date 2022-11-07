
#include <ultimate_esp_mqtt_ubidots.h>
#include "u_macros.h"
#include"MQ135.h"

esp_updater OTA_update;
esp_mqtt esp_ubidots(MQTT_CLIENT_NAME,TOKEN);
esp_timer mq135_timer(5,'m',"mq135");


/*** MQ 135 pins *****************/
MQ135 gasSensor = MQ135(A0);
float air_quality;

void setup()
{
  Serial.begin(115200);
  Serial.println("MQ135 Air Quality Project");

  OTA_update.start_wifi_manager();
  Serial.print("Firmware Version:");
  Serial.println(FIRMWARE_VERSION);

  //Check Update
  Serial.println("Checking For Firmware Update....");

  WiFiClient client;
  OTA_update.start_http_update(client,OTA_URL,FIRMWARE_VERSION);

  //Start MQTT connection
  esp_ubidots.init();

  //Start mq135 timer
  mq135_timer.start();

}

void loop()
{
    char temp_buff1[20];
    esp_ubidots.mqtt_loop();
    mq135_timer.timer_loop();


    //MQ135 func
    if( mq135_timer.timer_elapsed == true)
    {
        Serial.println("Sending MQ135 data");
        mq135_timer.timer_elapsed = false; 
        MQ135_loop();
        if(air_quality > 1)
        {
            sprintf(temp_buff1, "%s", "");
            sprintf(temp_buff1,"%f",air_quality);
            esp_ubidots.publish_data(HOME_AUTO_LABEL,AQI_LABEL,temp_buff1);
        }
        else{
            Serial.println("Wrong data");
        }
    }
}

void MQ135_loop()
{
    air_quality = ((analogRead(A0)/1024.0)*100.0);
    Serial.print("Air Quality: ");  
    Serial.print(air_quality);
    Serial.println("  PPM");   
    Serial.println();
}
