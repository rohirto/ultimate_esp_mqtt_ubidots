
#include <ultimate_esp_mqtt_ubidots.h>
#include "u_macros.h"
#include"MQ135.h"

/**** OTA Suff******************/
//#define FIRMWARE_VERSION  "v6"

/**** WiFi Stuff ***************/
// char* client_name = MQTT_CLIENT_NAME;
// const char* mqtt_server ="industrial.api.ubidots.com";
// char payload[700];
// char topic[150];

esp_updater OTA_update;
esp_mqtt esp_ubidots(MQTT_CLIENT_NAME,TOKEN);
esp_timer mq135_timer(5,'m',"mq135");


// WiFiClient ubidots;
// PubSubClient client(ubidots);

//bool connected  = false;

/*** Func Prototypes *************/
// void callback(char*, byte*, unsigned int);  //MQTT callback func
// void reconnect(void);     //reconnect to MQTT

/*** Timer variables *************/
// unsigned long startMillis;  //Some global vaiable anywhere in program
// unsigned long currentMillis;
// volatile byte ten_sec_counter = 0;
// volatile byte wifi_reconnect_interval = 3;
// volatile bool wifi_reconnect_elapsed = false;
 
// volatile bool boot_flag = false;

/*** MQ 135 pins *****************/
MQ135 gasSensor = MQ135(A0);
float air_quality;

void setup()
{
    Serial.begin(115200);
    Serial.println("MQ135 Air Quality Project");

    // WiFiManager wifiManager;
    // wifiManager.setConfigPortalTimeout(180);
    // wifiManager.autoConnect("Air Quality Sensor", "espflopflop");

    // while (WiFi.status() != WL_CONNECTED) {
    //     if(boot_flag == false) // First Boot
    //     {
    //         Serial.println("Connection Failed! Rebooting...");
    //         delay(5000);
    //         ESP.restart();
    //     }
    //     else   //Not first boot
    //     {
        
    //         //Recursively call the setup func
    //         setup();
    //     }
    // }

    // // OTA new mode 
    // ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW); // Optionnal
    // ESPhttpUpdate.rebootOnUpdate(false); // remove automatic update

    // String urlUPD = "http://192.168.1.100:8080/Firmware" + String(FIRMWARE_VERSION + 1) + ".bin";
    // Serial.print("Firmware version = ");
    // Serial.println(FIRMWARE_VERSION);
    // Serial.print("Next version = ");
    // Serial.println(FIRMWARE_VERSION + 1);



    // delay(2000);

    // t_httpUpdate_return ret = ESPhttpUpdate.update(ubidots, urlUPD);
    // switch (ret)
    // {
    //     case HTTP_UPDATE_OK:
    //         Serial.println("HTTP_UPDATE_OK. Reboot");
    //         delay(1000); // Wait a second and restart
    //         ESP.restart();
    //     break;
    //     case HTTP_UPDATE_NO_UPDATES:
    //         Serial.println("No updates present");
    //     break;
    //     case HTTP_UPDATE_FAILED:
    //         Serial.println("HTTP server connection failed");
    //     break;
    //     default:
    //         Serial.println("Default case");
    //     break;
    // }

    // //OTA Ends 
    // Serial.println("Ready");
    // Serial.print("IP address: ");
    // Serial.println(WiFi.localIP());

    // client.setServer(mqtt_server, 1883);
    // Serial.println("Server Set");
    // client.setCallback(callback);
    // Serial.println("Callback Set");

    // if(boot_flag == false)
    // {
    //      startMillis = millis();
    // }
    //  //First boot
    // if(boot_flag == false)
    // {
    //     boot_flag = true;
    // }

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
    // connected = client.connected();
    esp_ubidots.mqtt_loop();
    mq135_timer.timer_loop();
    // if(!connected)
    // {
    //     reconnect();
    // }
    // client.loop();
    // timer_function(); //Update Timers  

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

// bool actualUpdate(bool sketch=false){
//     String msg;
//     t_httpUpdate_return ret;

//     ESPhttpUpdate.rebootOnUpdate(false);
//     if(sketch){
//       ret=ESPhttpUpdate.update(updateUrl,TRUC_VERSION);     // **************** This is the line that "does the business"   
//     }
//     else {
//       ret=ESPhttpUpdate.updateFS(updateUrl,SPIFFS_VERSION);
//     }
// }

void MQ135_loop()
{
    air_quality = ((analogRead(A0)/1024.0)*100.0);
    Serial.print("Air Quality: ");  
    Serial.print(air_quality);
    Serial.println("  PPM");   
    Serial.println();
}

// void callback(char* topic, byte* payload, unsigned int length)
// {
//     Serial.print("Message arrived [");
//     Serial.print(topic);
//     Serial.print("] ");
//     char temp_buff[10];
//     for (int i = 0; i<10; i++)
//     {
//         temp_buff[i] = '\0';
//     }
//     for( int i = 0; i < length; i++)
//     {
//         temp_buff[i] = (char)payload[i];
//         Serial.print((char)payload[i]);  
//     }
//     Serial.println();
// }
// void reconnect() {
//   if(WiFi.status() != WL_CONNECTED && wifi_reconnect_elapsed == true)
//   {
//     //Wifi is not connected 
//     Serial.println("Reconnecting to WiFi...");
//     //WiFi.disconnect();
//     //WiFi.reconnect();
//     setup();
//     wifi_reconnect_elapsed = false;

//   }
//   else if(WiFi.status() == WL_CONNECTED)
//   {
//     //Wifi is connected
//      // Loop until we're reconnected
//     while (!client.connected()) {
//       Serial.println("Attempting MQTT connection...");
    
//       // Attempt to connect
//       if (client.connect(MQTT_CLIENT_NAME, TOKEN,"")) {
//         Serial.println("connected");
//       } else {
//         Serial.print("failed, rc=");
//         Serial.print(client.state());
//         Serial.println(" try again in 2 seconds");
//         // Wait 2 seconds before retrying
//         delay(2000);
//       }
//     }
//   }
// }
// void timer_function()
// {
//   currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
//   if ( currentMillis - startMillis >= 10000)
//   {
//     startMillis = currentMillis;
//     ten_sec_counter++;
//     mq135_timer_elapsed = true;

   
//     if ((ten_sec_counter % wifi_reconnect_interval) == 0) //test whether the period has elapsed
//     {
//       //temp_humd_timer_elapsed = true;
//       ten_sec_counter = 0;  //IMPORTANT to save the start time of the current LED state.
//       if(WiFi.status() != WL_CONNECTED)
//       {
//            wifi_reconnect_elapsed = true;
//       }
     
//     }
//   }

// }

// void mqtt_subscribe(char* mqtt_topic)
// {
//   //char *topicToSubscribe;
//   sprintf(topic, "%s", mqtt_topic);
//   client.subscribe(topic);
// }

// void publish_data(char* device_label, char* variable_label, char* payload_data)
// {
//   sprintf(topic, "%s", ""); // Cleans the topic content
//   sprintf(topic, "%s%s", "/v1.6/devices/", device_label);

//   sprintf(payload, "%s", ""); //Cleans the payload
//   sprintf(payload, "{\"%s\":", variable_label); // Adds the variable label   
//   sprintf(payload, "%s {\"value\": %s", payload, payload_data); // Adds the value
//   sprintf(payload, "%s } }", payload); // Closes the dictionary brackets

//   client.publish(topic, payload);
//   client.loop();
//   delay(1000);
// }
