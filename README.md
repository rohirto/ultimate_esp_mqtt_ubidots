# ultimate_esp_mqtt_ubidots

## Hardware
1. ESP8266 based boards: Node MCU, ESP 07, ESP 12F

## Description
Library Capable of:
1. WiFi Manager: Auto choose WiFi AP dynamically 
2. HTTP OTA: Web server based OTA (External NodeJS Server -> https://github.com/rohirto/OTA_Server)
3. Ubidots MQTT 

## Usage 
1. WiFi Manager Module: 
  ```
  esp_updater OTA_update;
  void setup(){
  OTA_update.start_wifi_manager();
  }
  ```
2. HTTP Updater: 
  
  In user define header 
  ```
  #define FIRMWARE_VERSION  "v10"
  #define OTA_URL           "URL according http update library of esp8266"
  ```
  
  In sketch
  ```
  esp_updater OTA_update;
  void setup(){
  OTA_update.start_wifi_manager();
  WiFiClient client;
  OTA_update.start_http_update(client,OTA_URL,FIRMWARE_VERSION);
  }
  ```
3. Ubidots MQTT:
  ```
  #define MQTT_CLIENT_NAME    "NodeMCUv2"
  #define TOKEN               "Ubidots Token"
  
  void setup(){
  //Start MQTT connection
  esp_ubidots.init();
  }
  void loop(){
  esp_ubidots.mqtt_loop();
  }
  ```
4. User defined Timers:
  
  ```
  esp_timer mq135_timer(5,'m',"mq135"); //5 min mq135 named timer, other options are 's' (seconds) and 'h' (hour)
  
  void setup(){
   mq135_timer.start();
   }
   
   void loop(){
   mq135_timer.timer_loop();
   if( mq135_timer.timer_elapsed == true){
    mq135_timer.timer_elapsed = false;
    }
   }
  ```
  
  
  
  
  

  
