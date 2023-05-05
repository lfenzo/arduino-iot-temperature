#include "WiFi.h"
#include "HTTPClient.h"
#include "time.h"
#include "DHT.h"
#include <stdio.h>



//
// DHT11 sensor settings
//
const int DHT11_SENSOR_PIN = 19;
DHT dht(DHT11_SENSOR_PIN, DHT11); // initialize the DHT11 sensor

//
// state alternating
//
int IS_TRANSMITTING_DATA = false;
int buttom_previous = 0;
int buttom_current = 1;

const int TOGGLE_BUTTOM_PIN = 33;
const int LED_PIN = 32;

//
// Local wifi settings
//
const char *SSID = "Casinha";
const char *PASSWORD = "5cJZXT9b5Hmb2FFcAmmQ";
const char *SERVER_ADDRESS = "http://192.168.0.14:8000";

//
// NTP setup settings
//
#define MAX_DATETIME_LENGTH 20
const char *NTP_SERVER = "pool.ntp.org";
const int gmt_offset_sec = -10800;  // local time is GMT-3 --> 3 * -3600 = -10.800
const int daylight_offset_sec = 0;
char DATETIME[MAX_DATETIME_LENGTH];



const char *DEVICE_ID = "esp32-dht11-1";



void setup() {

    Serial.begin(115200);
    pinMode(TOGGLE_BUTTOM_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    // setting up wifi for intermittent connection
    initialize_wifi();
    Serial.printf("Connected to WiFi. IP: %s\n", WiFi.localIP().toString().c_str());

    // initializing the used sensors
    dht.begin();

    // settingup the datetime configuration (it suffices to set this up only once because the
    // other datetime queries will be performed using the current local time).
    configTime(gmt_offset_sec, daylight_offset_sec, NTP_SERVER);
}


void loop() {
    verify_current_transmitting_behavior();

    if (IS_TRANSMITTING_DATA) {

        delay(2000);

        float temperature = dht.readTemperature();
        Serial.printf("temperature:\t %f\n", temperature);

        float humidity = dht.readHumidity();
        Serial.printf("humidity:\t %f\n", humidity);

        current_datetime_from_ntp(DATETIME, "%F %T", MAX_DATETIME_LENGTH);
        Serial.printf("datetime:\t %s\n", DATETIME);

        String json_query = "";
        json_query += "{";
        json_query += "\"device\":\"" + String(DEVICE_ID) + "\",";
        json_query += "\"datetime\":\"" + String(DATETIME) + "\",";
        json_query += "\"temperature\":\"" + String(temperature) + "\",";
        json_query += "\"humidity\":\"" + String(humidity) + "\"";
        json_query += "}";

        http_request("POST", String(SERVER_ADDRESS) + "/" + "sensor_observations/", json_query);
    }
}



void initialize_wifi() {
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}


bool http_request(String req_type, String url, String query) {

    if (!WiFi.status() == WL_CONNECTED) {
        Serial.printf("WiFi is not connected to network \"%s\".\n", SSID);
        return false;
    }

    HTTPClient http;
    int http_response_code;

    // GET request to the endpoint providing a query built-in
    if (req_type == "GET") {
        http.begin((url + query).c_str());
        http_response_code = http.GET();
    }
    // POST request to the endpoint passing the query as a parameter to the POST method
    else if (req_type == "POST") {
        http.begin(url.c_str());
        http.addHeader("Content-Type", "application/json");
        http_response_code = http.POST(query);
    }
        
    Serial.printf("HTTP Response code: %i\n", http_response_code);

    if (http_response_code > 0) {
        String payload = http.getString();
        Serial.println(payload);
    }

    http.end();

    return true;
}


void verify_current_transmitting_behavior() {
    buttom_current = digitalRead(TOGGLE_BUTTOM_PIN);

    if (buttom_current == 1 && buttom_previous == 0) {  // buttom pressed
        IS_TRANSMITTING_DATA = !IS_TRANSMITTING_DATA;
        digitalWrite(LED_PIN, IS_TRANSMITTING_DATA);
    }

    buttom_previous = buttom_current;
    delay(100);
}


/*
    Once set the NTP current time, this function will query the current time from the
    configuration and retrieve the current datetime in the specified '*fmt' using as sink
    the string pointed by '*str' (the 'length' of the string must also be specified).
*/
void current_datetime_from_ntp(char *str, char *fmt, int length) {
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }

    strftime(str, length, fmt, &timeinfo); 
}
