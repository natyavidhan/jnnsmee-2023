#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include "TinyGPS++.h"

const char *ssid = "";
const char *password = "";
const char *car = "";
const char *host = "";
const int httpsPort = 443;
const char *fingerprint = "AA C0 E2 66 DB D8 C5 9B E8 A7 2C 66 00 0D DF D2 4E D6 86 B0";
float mylat;
float mylon;
float D[10];

#define IRSENSOR_PIN 4
long newTime = millis();
long oldTime = millis();
bool lastRead = 0;
bool newRead = 0;
#define BLACK 0
#define WHITE 1
float Time;
float sped;

TinyGPSPlus gps;
SoftwareSerial serial_connection(14, 12);
// SoftwareSerial sped (5, 6);

void coor()
{
    smartdelay_gps(1000);
    if (gps.location.isValid())
    {
        mylat = (gps.location.lat(), 6);
        mylon = (gps.location.lng(), 6);
        Serial.println(mylat);
        Serial.println(mylon);
    }
}

void msg(float speed)
{
    coor();
    mylat = 28.6500;
    mylon = 77.2500;
    WiFiClientSecure httpsClient;
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000);
    delay(1000);

    Serial.print("HTTPS Connecting");
    int r = 0;
    while ((!httpsClient.connect(host, httpsPort)) && (r < 40))
    {
        delay(100);
        Serial.print(".");
        r++;
    }
    if (r == 40)
    {
        Serial.println("Connection failed");
    }
    else
    {
        Serial.println("Connected to web");
    }
    String Link = "/report?car=";
    Link += car;
    Link += "&lat=";
    Link += mylat;
    Link += "&lon=";
    Link += mylon;
    Link += "&speed=";
    Link += speed;

    Serial.print("requesting URL: ");
    Serial.println(host + Link);

    httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                      "Connection: close\r\n\r\n");

    Serial.println("request sent");

    while (httpsClient.connected())
    {
        String line = httpsClient.readStringUntil('\n');
        if (line == "\r")
        {
            Serial.println("headers received");
            break;
        }
    }
    Serial.println("closing connection");

    delay(10000);
}

void setup()
{
    pinMode(IRSENSOR_PIN, INPUT);
    Serial.begin(115200);
    WiFi.mode(WIFI_OFF);
    delay(500);
    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, password);
    Serial.println("");

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // msg(72);
}

static void smartdelay_gps(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (serial_connection.available())
            gps.encode(serial_connection.read());
    } while (millis() - start < ms);
}

void loop()
{
    newRead = digitalRead(IRSENSOR_PIN);
    if (newRead == BLACK && newRead != lastRead)
    {
        lastRead = newRead;
    }
    else if (newRead == WHITE && newRead != lastRead)
    {
        newTime = millis();
        lastRead = newRead;
        Time = (newTime - oldTime);
        oldTime = millis();
        sped = (6 / (Time / 1000));
        Serial.println(sped);
        if (sped > 50 && sped < 1000)
        {
            Serial.println(sped);
            msg(sped);
        }
    }
}
