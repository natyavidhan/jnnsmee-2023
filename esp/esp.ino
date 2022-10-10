#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include "TinyGPS++.h"
#include "LiquidCrystal_I2C.h"

const char *ssid = "JioFiber-ooCh7";
const char *password = "subal1234";
const char *car = "DL5SAB9876";
const char *host = "jnnsmee-2022.vercel.app";
const int httpsPort = 443;
const char *fingerprint = "D0 B6 8E BB EC 26 22 D9 2A BA D8 7F 62 E6 1A 6E 55 33 89 DE";
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

TinyGPSPlus gps;
SoftwareSerial serial_connection(5, 6);

void coor()
{
    while (serial_connection.available())
    {
        gps.encode(serial_connection.read());
    }
    if (gps.location.isUpdated())
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
//    mylat = 28.6500;
//    mylon = 77.2500;
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
    String Link  = "/report?car=" ;
    Link+= car ;
    Link+= "&lat="; 
    Link+= mylat ;
    Link+= "&lon="; 
    Link+= mylon ;
    Link+= "&speed=" ;
    Link+= speed;

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

    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    while (httpsClient.available())
    {
        line = httpsClient.readStringUntil('\n');
        Serial.println(line);
    }
    Serial.println("==========");
    Serial.println("closing connection");

    delay(2000);
}

void setup()
{
    delay(1000);
    Serial.begin(115200);
    WiFi.mode(WIFI_OFF);
    delay(1000);
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
    msg(72);
}

void loop()
{
//    newRead = digitalRead(IRSENSOR_PIN);
//    if (newRead == BLACK && newRead != lastRead)
//    {
//        lastRead = newRead;
//    }
//    if (newRead == WHITE && newRead != lastRead)
//    {
//        newTime = millis();
//        lastRead = newRead;
//        Time = (newTime - oldTime);
//        oldTime = millis();
//        float speed = (28 / Time) * 36;
//        Serial.print("Speed: " + speed);
//        if (speed > 50)
//        {
//            Serial.println("Overspeeding");
//            msg(speed);
//        }
//        else
//        {
//            Serial.println("Normal Speed");
//        }
//    }
}
