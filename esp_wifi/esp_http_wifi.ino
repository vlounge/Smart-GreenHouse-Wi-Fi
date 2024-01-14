#include <Arduino.h>

#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>


#define STASSID "Your Wi-Fi name"
#define STAPSK "Your Wi-Fi pass"
float parsingOk;
float dataChanged;

String msgFromGreenHouse="";
String oldData="";
String parsedReadings="";
String parsedStatuses="";
String tmp = "";
String brt = "";
String mst = "";
String htr = "";
String clr = "";
String lmp = "";
String irg = "";

int openBracket = 0;
int closeBracket = 0;


ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.print("Connecting Wi-Fi");
  delay(2000);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);
  Serial.println("setup() done connecting to ssid '" STASSID "'");
  Serial.println(WiFi.localIP());
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print(WiFi.localIP());
  delay(2000);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;


    HTTPClient http;


    if (http.begin(client,"Your link")) {

      int httpCode = http.GET();

      if (httpCode > 0) {

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
      
          Serial.print(payload.substring(1,payload.length()-2));
        }
      } else {

        Serial.print("Get failed");
      }
      http.end(); 
    } else {
      Serial.print("Server failed");
    }
    while (Serial.available()) {
      msgFromGreenHouse = Serial.readString();
    }
    parsingOk = false;
    dataChanged = false;

    if(oldData!=msgFromGreenHouse){
      oldData = msgFromGreenHouse;
      dataChanged = true;
      tmp = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("tmp")+4,msgFromGreenHouse.indexOf("clr"));
      brt = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("brt")+4,msgFromGreenHouse.indexOf("lmp"));
      mst = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("mst")+4,msgFromGreenHouse.indexOf("irg"));
      htr = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("htr")+4,msgFromGreenHouse.indexOf("brt"));
      clr = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("clr")+4,msgFromGreenHouse.indexOf("htr"));
      lmp = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("lmp")+4,msgFromGreenHouse.indexOf("mst"));
      irg = msgFromGreenHouse.substring(msgFromGreenHouse.indexOf("irg")+4,msgFromGreenHouse.indexOf("end"));

      parsedReadings = "";
      parsedReadings.concat("{\"api_key\":\"Your key\",");
      parsedReadings.concat("\"humidity\":\"");
      parsedReadings.concat(mst);
      parsedReadings.concat("\",");
      parsedReadings.concat("\"light\":\"");
      parsedReadings.concat(brt);
      parsedReadings.concat("\",");
      parsedReadings.concat("\"temperature\":\"");
      parsedReadings.concat(tmp);
      parsedReadings.concat("\"}");
      parsedStatuses = "";
      parsedStatuses.concat("{\"api_key\":\"Your key\",");
      parsedStatuses.concat("\"heating\":\"");
      parsedStatuses.concat(htr);
      parsedStatuses.concat("\",");
      parsedStatuses.concat("\"cooling\":\"");
      parsedStatuses.concat(clr);
      parsedStatuses.concat("\",");
      parsedStatuses.concat("\"lighting\":\"");
      parsedStatuses.concat(lmp);
      parsedStatuses.concat("\",");
      parsedStatuses.concat("\"watering\":\"");
      parsedStatuses.concat(irg);
      parsedStatuses.concat("\"}");
      parsingOk = true;

    }
    if (parsingOk && dataChanged){
      if (http.begin(client,"Your link")) {
        http.addHeader("Content-Type", "application/json");
        http.addHeader("auth-key", "Your key");
        int httpResponseCode = http.PUT(parsedReadings);
        
        if(httpResponseCode>0){
          String response = http.getString();          
        }else{
        }
        http.end();
      } else {

      }
      if (http.begin(client,"Your link")) {
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.PUT(parsedStatuses);
        http.end();
      } else {
        Serial.print("Client 2 failed");
      }
        Serial.print(mst+","+brt+","+tmp+","+htr+","+clr+","+lmp+","+irg);
    }
  }

  delay(60000);
}
