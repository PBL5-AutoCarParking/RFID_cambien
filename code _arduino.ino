// #include<SoftwareSerial.h>
// SoftwareSerial espSerial(2,3);  // 0,1 để giao tiếp với Serial monitor rồi
// Khai báo chân kết nối cảm biến hồng ngoại
const int ir_entrance1 = 4;
const int ir_entrance2 = 5;

const int ir_exit1 = 8;
const int ir_exit2 = 9;

void setup() {
  // Khởi tạo giao tiếp serial với tốc độ 9600 bps
  Serial.begin(9600);
  // espSerial.begin(9600);
  // espSerial.begin(9600, SWSERIAL_8N1);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT);
}

void loop() {
  // Đọc giá trị từ cảm biến hồng ngoại thứ nhất
  int infraredValue1 = digitalRead(ir_entrance1);

  // Đọc giá trị từ cảm biến hồng ngoại thứ hai
  int infraredValue2 = digitalRead(ir_entrance2);

  int exitValue1 = digitalRead(ir_exit1);

  // Đọc giá trị từ cảm biến hồng ngoại thứ hai
  int exitValue2 = digitalRead(ir_exit2);

  // espSerial.print("CV1=");
  // espSerial.println(infraredValue1);

  // espSerial.print("CV2=");
  // espSerial.println(infraredValue2);

  // espSerial.print("CR1=");
  // espSerial.println(exitValue1);

  // espSerial.print("CR2=");
  // espSerial.println(exitValue2);

  // Hiển thị giá trị từ cảm biến trên Serial Monitor
  if(infraredValue1==0){
      Serial.print("CV1=");
      Serial.println(infraredValue1);
  }
  if(infraredValue2==0){
      Serial.print("CV2=");
      Serial.println(infraredValue2);
  }

  if(exitValue1==0){
      Serial.print("CR1=");
      Serial.println(exitValue1);
  }

  if(exitValue2==0){
      Serial.print("CR2=");
      Serial.println(exitValue2);
  }

  delay(1000); // Đợi 1 giây
}

