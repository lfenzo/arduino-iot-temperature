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
const String SERVER_ADDRESS = "http://192.168.0.19:8000";

//
// NTP setup settings
//
const char *NTP_SERVER = "pool.ntp.org";
const int gmt_offset_sec = -10800;  // local time is GMT-3 --> 3 * -3600 = -10.800
const int daylight_offset_sec = 0;






void setup() {
    Serial.begin(115200);
    pinMode(TOGGLE_BUTTOM_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    //initialize_wifi();
    dht.begin();
}

void loop() {
    verify_current_transmitting_behavior();
    Serial.println(IS_TRANSMITTING_DATA);

    Serial.print("temperature\t");
    Serial.println(dht.readTemperature());
    delay(2000);
}



void initialize_wifi() {
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWifi connected...");
}



//bool http_get(String server, String endpoint, String query) {
//
//    if (!WiFi.status() == WL_CONNECTED) {
//        Serial.println("WiFi not connected");
//        return false;
//    }
//
//    String URL = server + endpoint;
//    Serial.println(URL);
//
//    HTTPClient http;
//    http.begin(URL.c_str());
//
//    int http_response_code = http.GET();
//
//    Serial.print("HTTP Response code: ");
//    Serial.println(http_response_code);
//
//    if (http_response_code > 0) {
//        String payload = http.getString();
//        Serial.println(payload);
//    }
//    else {
//        Serial.print("Error code: ");
//    }
//
//    // Free resources
//    http.end();
//
//    return true;
//}


void verify_current_transmitting_behavior() {
    buttom_current = digitalRead(TOGGLE_BUTTOM_PIN);

    if (buttom_current == 1 && buttom_previous == 0) {  // buttom pressed
        IS_TRANSMITTING_DATA = !IS_TRANSMITTING_DATA;
        digitalWrite(LED_PIN, IS_TRANSMITTING_DATA);
        Serial.println(WiFi.localIP());
    }

    buttom_previous = buttom_current;
    delay(100);
}


void current_datetime_from_ntp(String format) {
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    //Serial.println(&timeinfo, format);
}
