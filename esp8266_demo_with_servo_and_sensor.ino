#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h> // Thêm thư viện Servo

// #include <SoftwareSerial.h>
// SoftwareSerial arduinoSerial(3,1);



#define SS_PIN D8  // D2
#define RST_PIN D0 // D1

// const int irSensorPin = D4; 
#define servoEntrancePin D2 // Chỉ định chân kết nối servo
#define servoExitPin D4

MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

WiFiClient client;
HTTPClient http;

  String paramName="";
  String paramValue="";

const char* serverURL = "http://192.168.197.68:8000";  // Thay thế bằng URL của máy chủ của bạn

Servo servoEntrance; // Khởi tạo đối tượng Servo cho cổng vào
Servo servoExit;     // Khởi tạo đối tượng Servo cho cổng ra

void setup() {
  Serial.begin(9600); // Kết nối với Serial Monitor
  // arduinoSerial.begin(9600); // Kết nối với Arduino
  // pinMode(irSensorPin, INPUT);
  SPI.begin();
  mfrc522.PCD_Init();  // Khoi tao MFRC522
  Serial.println();
  WiFi.begin("Redmi Note 12", "myredmi12");  // Thay thế bằng SSID và mật khẩu của mạng WiFi của bạn

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  // Khởi tạo Servo
  servoEntrance.attach(servoEntrancePin);
  servoExit.attach(servoExitPin);
  servoEntrance.write(0);
  servoExit.write(0);
  Serial.println("Finish setup");
}

void loop() {

  int value=2;
  if (Serial.available()) {
    // Đọc dữ liệu từ Arduino
    String data = Serial.readStringUntil('\n');

    // In dữ liệu nhận được
    Serial.print("Received data: ");
    Serial.println(data);
    // Xử lý chuỗi để lấy giá trị số nguyên
      // Tìm vị trí của ký tự '=' trong chuỗi
    int equalSignIndex = data.indexOf('=');

    if (equalSignIndex != -1) {
      // Tách phần cv1 và giá trị từ chuỗi
      paramName = data.substring(0, equalSignIndex);
      paramValue = data.substring(equalSignIndex + 1);

      // In phần cv1 và giá trị
      Serial.print("Parameter: ");
      Serial.println(paramName);
      Serial.print("Value: ");
      Serial.println(paramValue);

      // Kiểm tra nếu paramValue có thể chuyển đổi thành giá trị số nguyên
      if (paramValue.toInt()) {
        // Chuyển đổi paramValue thành giá trị số nguyên và in ra
        value = paramValue.toInt();
        // Serial.print("Parsed value: ");
        Serial.println(value);
      }
        // Serial.print("CV2:");
        // Serial.print(paramName=="CV2");
      if(paramName=="CV2"){
        Serial.println("Close entrance barrier");
        servoEntrance.write(0);
        delay(1000);
      }
      delay(100);
      if(paramName=="CR2"){
        Serial.println("Close exit barrier");
        servoExit.write(0);
        delay(1000);
      }
    }
  }
  // int irValue = digitalRead(irSensorPin);

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  delay(2000);
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
  delay(100);

  if(paramName=="CV1"){
    bool entry_signal=true;
    if (sendDataToServer(content,entry_signal)) {
      Serial.println("Data sent successfully");
    }else{
      Serial.println("Error in sending to entrance");
    }  
  } 
  delay(100);
  if(paramName=="CR1"){
    if (sendDataToServer(content,false)) {
      Serial.println("Data for exit sent successfully");
    } 
    else{
      Serial.println("Error in sending to entrance");
    }    
  }


  delay(2000);  // Chờ 2 giây trước khi quét thẻ tiếp theo
}

bool sendDataToServer(String data,bool entry_signal) {
  // Tạo URL POST request
  String url = serverURL;
  url += "/process_post";  // Thay thế bằng đường dẫn API của bạn

  // Đặt nội dung dữ liệu trong yêu cầu HTTP POST
  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Gửi dữ liệu
  if(entry_signal)
  {
    int httpResponseCode = http.POST("uuid=" + data+"&entry_signal="+"True");  // Thay thế "data" bằng tên tham số trong yêu cầu POST của bạn
    http.end();
    if (httpResponseCode == HTTP_CODE_OK) {
      Serial.println("Xoay servo");
      servoEntrance.write(180);
      return true;
    }
    else{
      Serial.print("HTTP POST failed, error code: ");
      Serial.println(httpResponseCode);
      return false;
    }
  }else{
    int httpResponseCode = http.POST("uuid=" + data+"&entry_signal="+"False");
    http.end();
    if (httpResponseCode == HTTP_CODE_OK) {
      Serial.println("Xoay servo");
      servoExit.write(180);
      return true;
    }
    else{
      Serial.print("HTTP POST failed, error code: ");
      Serial.println(httpResponseCode);
      return false;
    }
  }
  // Đóng kết nối và trả về kết quả
}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}