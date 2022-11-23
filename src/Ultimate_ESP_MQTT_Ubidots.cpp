#include "Ultimate_ESP_MQTT_Ubidots.h"

ESP8266WiFiMulti WiFiMulti;
volatile bool boot_flag = false;
// MQTT Client
WiFiClient ubidots;
PubSubClient client(ubidots);
bool wifi_elapsed = false;
esp_timer wifi_reconnect_timer(3, 'm', "wifi"); // Timer of 3 min
esp_timer ntp_update_timer(1,'m',"NTP_timer");
//NTP Stuff
esp_ntp ntpC;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", ntpC.utcOffsetInSeconds);

void update_started()
{
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished()
{
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total)
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err)
{
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void esp_updater::start_wifi_manager()
{
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect("OTA updater", "espflopflop");
  while (WiFi.status() != WL_CONNECTED)
  {
    if (boot_flag == false) // First Boot
    {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
    else // Not first boot
    {

      // Recursively call the setup func
      this->start_wifi_manager();
    }
  }

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void esp_updater::start_http_update(String url, String version)
{
 
  updater( version, url);
}


void updater(String version, String url)
{
  if ((WiFi.status() == WL_CONNECTED))
  {

    // WiFiClient client;
    Serial.setDebugOutput(true);
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    ESPhttpUpdate.rebootOnUpdate(false); // remove automatic update

    Serial.println(F("Update start now!"));
    // t_httpUpdate_return ret = ESPhttpUpdate.update("http://server/file.bin");
     WiFiClient up_client;
    t_httpUpdate_return ret = ESPhttpUpdate.update(up_client, url, version);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      delay(1000); // Wait a second and restart
      ESP.restart();
      break;
    }
    delay(20000);
  }
}

esp_mqtt::esp_mqtt(char *clientName, char *ubidotsToken, bool u_debug)
{
  this->client_name = clientName;
  this->ubidots_token = ubidotsToken;
  this->server_name = MQTT_SERVER;
  this->debug = u_debug;
}

void esp_mqtt::init()
{
  client.setServer(server_name, 1883);
  if(this->debug == true)
    Serial.println("MQTT Server Set");
  client.setCallback(callback);
  if(this->debug == true)
    Serial.println("Callback Set");
  connected = false;
  resubscribe = true;
  if (boot_flag == false)
  {
    // Ideally should start timer here
    // wifi_reconnect_timer.start();
    timeClient.begin();
  }
  // First boot
  if (boot_flag == false)
  {
    boot_flag = true;
  }
}

void esp_mqtt::mqtt_loop()
{
  wifi_reconnect_timer.timer_loop();
  
  if (WiFi.status() == WL_CONNECTED && wifi_reconnect_timer.timer_elapsed == true)
  {
    // Clear wifi reconnect timer
    wifi_reconnect_timer.timer_elapsed = false;
    //Test
    ntpC.get_time_date();
  }
  connected = client.connected();
  if (!connected)
  {
    resubscribe = true;
    this->reconnect();
  }
  client.loop();

  ntp_update_timer.timer_loop();
  if(ntp_update_timer.timer_elapsed == true)
  {
    ntp_update_timer.timer_elapsed = false;
    timeClient.update();
  }
}

void callback(char *topic_t, byte *payload_t, unsigned int length_t)
{
  mqtt_user_code(topic_t,payload_t, length_t);
}
void esp_mqtt::reconnect()
{
  if (WiFi.status() != WL_CONNECTED && wifi_reconnect_timer.timer_elapsed == true)
  {
    // Wifi is not connected
    if(this->debug == true)
      Serial.println("Reconnecting to WiFi...");
    // WiFi.disconnect();
    // WiFi.reconnect();
    setup(); // Go back to Setup
    wifi_reconnect_timer.timer_elapsed = false;
  }
  else if (WiFi.status() == WL_CONNECTED)
  {
    // Wifi is connected
    //  Loop until we're reconnected
    if (!client.connected())
    {
      if(this->debug == true)
        Serial.println("Attempting MQTT connection...");

      // Attempt to connect
      if(this->debug == true)
      {
        Serial.print("Attempting to connect to:");
        Serial.println(this->client_name);
        Serial.println(this->ubidots_token);
      }
      if (client.connect(this->client_name, this->ubidots_token, ""))
      {
        if(this->debug == true)
          Serial.println("connected");

        // Ideally should re - subscribe here
        connected = true;
      }
      else
      {
        connected = false;
        if(this->debug == true)
        {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 2 seconds");
        }
        // Wait 2 seconds before retrying
        delay(2000);
      }
    }
  }
}

void esp_mqtt::mqtt_subscribe(const char *szTypes, ...)
{
  if (connected == true && resubscribe == true)
  {
    va_list vl;
    /* initialize valist for num number of arguments */

    va_start(vl, szTypes);

    while (szTypes)
    {
      sprintf(topic, "%s", szTypes);
      if (szTypes != NULL)
      {
        client.subscribe(topic);
        delay(1000);
      }
      szTypes = va_arg(vl, const char *);
      
    }

    va_end(vl);

    resubscribe = false;
  }
}

void esp_mqtt::publish_data(char *device_label, char *variable_label, char *payload_data)
{
  sprintf(topic, "%s", ""); // Cleans the topic content
  sprintf(topic, "%s%s", "/v1.6/devices/", device_label);

  sprintf(payload, "%s", "");                                   // Cleans the payload
  sprintf(payload, "{\"%s\":", variable_label);                 // Adds the variable label
  sprintf(payload, "%s {\"value\": %s", payload, payload_data); // Adds the value
  sprintf(payload, "%s } }", payload);                          // Closes the dictionary brackets

  client.publish(topic, payload);
  client.loop();
  delay(1000);
}

esp_timer::esp_timer(byte timer_interval, char unit, char *name)
{
  uint32_t t_interval;
  this->timer_name = name;
  switch (unit)
  {
  case 'm':
  case 'M':
    t_interval = timer_interval * 60000;
    init(t_interval);
    break;

  case 's':
  case 'S':
    t_interval = timer_interval * 1000;
    init(t_interval);
    break;

  case 'h':
  case 'H':
    t_interval = timer_interval * 60000 * 60;
    init(t_interval);
    break;

  default:
    break;
  }
}

void esp_timer::init(uint32_t interval)
{
  this->timer_interval = interval;
  this->timer_elapsed = false;
}

void esp_timer::timer_loop()
{
  currentMillis = millis();                            // get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= (timer_interval)) // Timer elapsed
  {
    startMillis = currentMillis;
    this->timer_elapsed = true;
    // Serial.print(this->timer_name);
    // Serial.print(" ");
    // Serial.println("Timer elapsed");
  }
}

void esp_timer::start()
{
  this->startMillis = millis();
}

esp_ntp::esp_ntp(){
}


void esp_ntp::get_time_date(){
  current_hr = timeClient.getHours();
  current_min = timeClient.getMinutes();
  Serial.print("Getting Time: ");
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
}