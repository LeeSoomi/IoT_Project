//Dashboard와 연동시키기 위한 변수 설정
  float co2;
  float dust;
  float gas;
  float humidity;
  float pressure;
  float temperature;
  int light;
  bool co2_alert_status;
  bool dust_alert_status;
  bool gas_alert_status;


// IoTCloud를 사용하기 위한 라이브러리와 함수
#include "thingProperties.h"
#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"

//제어부인 MKR IoT Carrier 사용 설정
MKRIoTCarrier carrier;

// 센서 핀 설정
#define co2_pin A4
#define gas_pin A5


// 미세먼지 센서핀과 제어를 위한 변수 설정
int dust_in = A6 ;
int ledPower = 2 ;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;



void setup() {
  // Initialize serial and wait for port to open:
  pinMode(ledPower, OUTPUT);
  analogReference(AR_DEFAULT) ;

  Serial.begin(9600);
  delay(1500);
  
  //IoT Cloud 사용되는 기본 함수
  initProperties();
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    delay(500);
  }
  delay(500);
  CARRIER_CASE = false ; //케이스 사용 안함
  carrier.begin(); 
  carrier.display.setRotation(0); 
  delay(1500);
}

void loop() {
  //IoT Cloud 사용시 호출되는 기본 함수
  ArduinoCloud.update();
  
  // 만들어져있는 함수 호출
  readSensors() ;
  sensor_Data() ;
  displayData() ;
  delay(100);
}

void readSensors() {
  // MKR IoT Carrier 내장센서인 온도, 습도, 기압센서값 읽어오기
  temperature = carrier.Env.readTemperature(); 
  humidity = carrier.Env.readHumidity();
  pressure = carrier.Pressure.readPressure();
}


void sensor_Data() {
  // Co2 센서값 읽기
  int sensorValue = analogRead(co2_pin);
  float voltage = sensorValue * (5000 / 1024.0);
  int voltage_diference = voltage - 400;
  co2 = voltage_diference * 50.0 / 16.0;

  // Co2값이 기준치 이상이면 화면에 LED색 바꾸기
  if(co2 >= 3000){
    co2_alert_status = 0 ;
  }
  else{
    co2_alert_status = 1  ;
  }
  

  // Gas 센서값 읽기
  gas = analogRead(gas_pin) ;
 
  // Gas 값이 기준치 이상이면 화면에 LED색 바꾸기
  if(gas >= 200){
    gas_alert_status = 0 ;
  }
  else{
    gas_alert_status = 1  ;
  }


  // 미세먼지센서 값 계산
  digitalWrite(ledPower,LOW);                
  delayMicroseconds(samplingTime);           
  
  voMeasured = analogRead(dust_in);       
  
  delayMicroseconds(deltaTime);              
  digitalWrite(ledPower,HIGH);               
  delayMicroseconds(sleepTime);              
  
  calcVoltage = voMeasured * (5.0 / 1024.0);      
  
  dustDensity = (0.17 * calcVoltage - 0.1) *1000;
  dust = dustDensity + 100 ;


  // 미세먼지값이 기준치 이상이면 화면에 LED색 바꾸기
  if(dust >= 150){
      dust_alert_status = 0 ;
    }
  else{
    dust_alert_status = 1  ;
    }
  
  delay(1000);
}


void displayData() {
  //MKR IoT Carrier의 내장센서인 터치센서를 누르면 스크린에 온도, 습도, 기압을 보여준다.
  // 터치 0 > 온도값
  // 터치 1 > 습도값
  // 터치 2 > 기압값
  
  carrier.Buttons.update();
  readSensors() ;
  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    carrier.display.fillScreen(ST77XX_WHITE);
    carrier.display.setTextColor(ST77XX_RED);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(30, 110);
    carrier.display.print("Temp: ");
    carrier.display.print(temperature);
    carrier.display.print(" C");
  }

  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    carrier.display.fillScreen(ST77XX_WHITE);
    carrier.display.setTextColor(ST77XX_RED);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(30, 110);
    carrier.display.print("Humi: ");
    carrier.display.print(humidity);
    carrier.display.print(" %");
  }
  if (carrier.Buttons.onTouchDown(TOUCH2)) {
    carrier.display.fillScreen(ST77XX_WHITE);
    carrier.display.setTextColor(ST77XX_RED);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(30, 110);
    carrier.display.print("Pressure: ");
    carrier.display.print(pressure);
  }
}
