#include "DHT.h"
#include "WiFi.h"
#include "HTTPClient.h"

#define DHTPIN 4
#define DHTTYPE DHT22

const char* ssid = "Pixel5";
const char* password = "1sampai9";

const char* apiKey = "2SAW458OUTNDDLGW";
const char* server = "http://api.thingspeak.com/update";

unsigned long lastSendTime = 0;
const unsigned long interval = 15 * 1000;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Memulai ESP32");
  dht.begin();

  WiFi.begin(ssid,password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  Serial.print("Connected to WiFi");
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= interval || lastSendTime == 0) {
    kirimData();
    lastSendTime = currentTime;
  } 
}

void kirimData() {
  float c = dht.readTemperature();
  float h = dht.readHumidity();

  if(isnan(c) || isnan(h)){
    Serial.print("Gagal membaca sensor DHT");
    return;
  }

  Serial.print("Suhu: "); Serial.println(c);
  Serial.print("Kelembaban: "); Serial.println(h);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(c) + "&field2=" + String(h);
    http.begin(url); //khusus esp32

    int httpCode = http.GET();

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Hasil kirim data: " + response);
    }
    else {
      Serial.print("Gagal kirim data ke ThingsSpeak");
    }
    http.end();
  } else {
    Serial.print("WiFi Terputus");
  }
}
