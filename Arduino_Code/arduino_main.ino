#include <Wire.h>

// 핀 설정
const int ledPin = 2; // 비상 경고등 (LED)
const int pinN = 5;   // 모터 기어 N
const int pinP = 6;   // 모터 기어 P
const int pinEN = 7;  // 모터 엑셀

uint32_t timeCheck = 0; 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(ledPin, OUTPUT);
  pinMode(pinN, OUTPUT);
  pinMode(pinP, OUTPUT);
  pinMode(pinEN, OUTPUT);

  // 초기 상태: 모두 끄기
  digitalWrite(ledPin, LOW); 
  digitalWrite(pinEN, LOW); 
  digitalWrite(pinP, LOW);
  digitalWrite(pinN, LOW);
  
  Serial.println("\r\nStart Smart Warehouse"); 
}

void loop() {
  // 1. 우분투 리눅스의 명령 수신 (독립 제어)
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    //1-1.LED 제어 명령
    if (cmd == 'L') {
      digitalWrite(ledPin, HIGH); // LED 켬
    } else if (cmd == 'l') {
      digitalWrite(ledPin, LOW);  // LED 끔
    } 
    //1-2.모터 제어 명령
    else if (cmd == 'M') {
      digitalWrite(pinP, HIGH);  
      digitalWrite(pinN, LOW);
      digitalWrite(pinEN, HIGH); // 모터 켬
    } else if (cmd == 'm') {
      digitalWrite(pinEN, LOW);  // 모터 끔
    }
  }

  // 2. 3초마다 온습도 측정 및 전송
  if (millis() > timeCheck) {
    timeCheck = millis() + 3000; 

    uint16_t rawTemp, rawHum;
    float temp, humi;

    Wire.beginTransmission(0x40); 
    Wire.write(0xF3);
    Wire.endTransmission();
    delay(85); 

    Wire.requestFrom(0x40, 3);
    if(Wire.available() >= 2) {
      rawTemp = Wire.read() << 8;
      rawTemp |= Wire.read();
      Wire.read(); 
      temp = -46.85 + 175.72 * (rawTemp / 65536.0); 
    }

    Wire.beginTransmission(0x40);
    Wire.write(0xF5);
    Wire.endTransmission();
    delay(29); 

    Wire.requestFrom(0x40, 3);
    if(Wire.available() >= 2) {
      rawHum = Wire.read() << 8;
      rawHum |= Wire.read();
      Wire.read(); 
      rawHum &= ~0x0003; 
      humi = -6.0 + 125.0 * (rawHum / 65536.0); 
    }

    Serial.print("T:");
    Serial.print(temp, 1); 
    Serial.print(",H:");
    Serial.println(humi, 1);
  }
}
