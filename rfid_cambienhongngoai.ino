#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SS_PIN 4  // D2
#define RST_PIN 5 // D1

const int irSensorPin = D4; 

MFRC522 mfrc522(SS_PIN, RST_PIN);
WiFiClient client;
HTTPClient http;

const char* serverURL = "http://192.168.1.59";  // Thay thế bằng URL của máy chủ của bạn

void setup() {
  Serial.begin(9600);
  pinMode(irSensorPin, INPUT);
  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin("LUCKY CAT 5G", "88888888");  // Thay thế bằng SSID và mật khẩu của mạng WiFi của bạn

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  int irValue = digitalRead(irSensorPin);

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println();
  Serial.print("UID tag: ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  Serial.println();

  // Gửi dữ liệu lên máy chủ
  if (sendDataToServer(irValue,content)) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Failed to send data");
  }

  delay(2000);  // Chờ 2 giây trước khi quét thẻ tiếp theo
}

bool sendDataToServer(int irValue, String data) {
  // Tạo URL POST request
  String url = serverURL;
  url += "/process_post";  // Thay thế bằng đường dẫn API của bạn

  // Đặt nội dung dữ liệu trong yêu cầu HTTP POST
  http.begin(client, url);
  http.addHeader("Content - Type", "application / x - www - form - urlencoded");

  // Gửi dữ liệu
  int httpResponseCode = http.POST("uuid=" + data + "irValue"+ String(irValue));  // Thay thế "data" bằng tên tham số trong yêu cầu POST của bạn
  Serial.println(httpResponseCode);
  // Đóng kết nối và trả về kết quả
  http.end();
  
  if (httpResponseCode == HTTP_CODE_OK) {
    return true;
  } else {
    Serial.print("HTTP POST failed, error code: ");
    Serial.println(httpResponseCode);
    return false;
  }
}