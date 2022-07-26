/*************************************************/
/* Debugging                                     */
/*************************************************/
constexpr bool debugOutput {true};  // set to true for serial OUTPUT

/*************************************************/
/* Settings for WLAN                             */
/*************************************************/
constexpr char* ssid {"KVSF"};
constexpr char* password {"einmalimKreis"};

/*************************************************/
/* Pin config                                    */
/*************************************************/
constexpr int input_pin_wind  {2};
constexpr int output_pin_Relay_1 {14};
constexpr int output_pin_Relay_2 { 12 };
constexpr int DS18B20_PIN { 4 };     // Pin for DS18B20 on Arduino D2   
constexpr int TEMPERATURE_PRECISION { 9 };

/*************************************************/
/* max wind speed                                */
/*************************************************/
constexpr int Config_max_wind_speed  {60};
constexpr int Config_max_max_wind_speed {70};
constexpr int Config_wind_hyst {10};
constexpr int Config_timeMaxMaxWindSpeed {300}; // time to open the blinds
constexpr int Config_timeMaxWindSpeed {60};     // time delay for the max wind speed limit, if the time is elapsed open the blinds
/*************************************************/
/* OTA Update settings (Over the air update)     */
/*************************************************/ 
constexpr char* firmware_version = "myanemometer_0.0.1";
constexpr char* update_server = "myhost";
constexpr char* update_uri = "/path/update.php";

/*************************************************/
/* MQTTCloud data                               */
/*************************************************/
constexpr char* mqtt_host { "192.168.178.37" };
constexpr char* mqtt_id { "ESP8266-Anemometer" };
constexpr char* mqtt_topic_prefix { "/weatherstation/wind" };
constexpr int mqtt_port { 1883 };
