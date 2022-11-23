# ultimate_esp_mqtt_ubidots

## Hardware
1. ESP8266 based boards: Node MCU, ESP 07, ESP 12F

## Description
Library Capable of:
1. WiFi Manager: Auto choose WiFi AP dynamically 
2. HTTP OTA: Web server based OTA (External NodeJS Server -> https://github.com/rohirto/OTA_Server)
3. Ubidots MQTT 
4. Wifi Reconnect timer and User Timers: A wifi reconnect timer of 3 mins (If wifi connection goes, either AP is turned off or Internet gone, the esp tries to reconnect with Ubidots server for 3 mins, after that if WiFi Status is of disconnected then esp goes back to WiFi manager mode to reconfigure a new AP -> which if not done in 3 mins will result in attempt for connection with existing AP -> the cycle goes on)

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
  OTA_update.start_http_update(OTA_URL,FIRMWARE_VERSION);
  }
  ```
3. Ubidots MQTT: In MQTT subscribe func, last argument should always be NULL, there should always be stub function mqtt_user_code(char* payload)
  ```
  #define MQTT_CLIENT_NAME    "NodeMCUv2"
  #define TOKEN               "Ubidots Token"
  #define MQTT_TOPIC          "MQTT Topic to subscribe"
  #define DEVICE_LABEL        "given by ubidots, used for publishing"
  #define VARIABLE_LABEL      "given by ubidots, used for publishing"
  
  esp_mqtt esp_ubidots(MQTT_CLIENT_NAME, TOKEN, DEBUG_FALSE);  
  void setup(){
  //Start MQTT connection
  esp_ubidots.init();
  esp_ubidots.mqtt_subscribe(MQTT_TOPIC1, MQTT_TOPIC2, NULL);  //Can dynamically enter the arguments, must be terminated with NULL
  }
  void loop(){
  esp_ubidots.mqtt_loop();
  esp_ubidots.mqtt_subscribe(MQTT_TOPIC1, MQTT_TOPIC2, NULL);  //need to keep resubscribing in case of disconnection
  }
  
  void mqtt_user_code(char* m_topic, byte* payload, int len){
  //Stub Function must be there
  //The IO changes which needs to change on message reception from MQTT server
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
  
  
  
  
  

  
