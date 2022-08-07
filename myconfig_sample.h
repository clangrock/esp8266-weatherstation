/*************************************************/
/* Debugging                                     */
/*************************************************/
constexpr bool debugOutput    {false};  // set to true for serial OUTPUT
constexpr bool debugError     {true};   // set to true for serial OUTPUT
constexpr bool debugMeasure   {true};   // set to true for serial OUTPUT

/*************************************************/
/* Settings for WLAN                             */
/*************************************************/
constexpr char* ssid        {"xxxxxx"};
constexpr char* password    {"xxxx"};

/*************************************************/
/* Pin config                                    */
/*************************************************/
constexpr int input_pin_wind        {12};
constexpr int output_pin_Relay_1    {14};
constexpr int output_pin_Relay_2    {13};
constexpr int DS18B20_PIN           {4};     // Pin for DS18B20 on Arduino D2   
constexpr int TEMPERATURE_PRECISION {9};
constexpr int DIGITAL_PIN_RAIN      {5};
constexpr int ANALOG_PIN            {0};
constexpr int LED_BUILTIN_1         {2};  // intern LED

/*************************************************/
/* max wind speed                                */
/*************************************************/
constexpr int Config_max_wind_speed         {55};
constexpr int Config_max_max_wind_speed     {70};
constexpr int Config_wind_hyst              {10};
constexpr int Config_timeMaxMaxWindSpeed    {300}; // time to open the blinds
constexpr int Config_timeMaxWindSpeed       {60};     // time delay for the max wind speed limit, if the time is elapsed open the blinds
/*************************************************/
/* OTA Update settings (Over the air update)     */
/*************************************************/ 
constexpr char* firmware_version  {"myanemometer_0.8.1"};
constexpr char* update_server     {"myhost"};
constexpr char* update_uri        {"/path/update.php"};

/*************************************************/
/* MQTTCloud data                               */
/*************************************************/
constexpr char* mqtt_host                       { "xxx.xxx.xxx.xxx" };
constexpr char* mqtt_id                         { "ESP8266-Anemometer" };
constexpr char* mqtt_topic_wind                 { "/weatherstation/wind" };
constexpr char* mqtt_topic_temperature          {"/weatherstation/temp_"}; 
constexpr char* mqtt_topic_temperature_sensors  {"/weatherstation/temp_Number"}; 
constexpr char* mqtt_topic_rain                 {"/weatherstation/rainsensor/rainning"}; 
constexpr char* mqtt_topic_rain_Moisture        {"/weatherstation/rainsensor/Moisture"}; 
constexpr int   mqtt_port                       { 1883 };};
