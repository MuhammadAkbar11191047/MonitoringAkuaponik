#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "GravityTDS.h"

const int pHSensorPin = A2;        
const float pHtujuh = 2.6;
const float pHstep = 2.7; 
const int TDSSensorPin = A0;        
const float VREF = 5.0;     
const int turbiditySensorPin = A4;  
const int ultrasonicTrigPin = 30;    
const int ultrasonicEchoPin = 26;   
float tinggiAir, tinggiAirPrev = 0;
const int jarakSensorDasarAir = 120;
const int batasMinPemSensor = 0;
const int ds18b20SensorPin = 22;     
const int pinPompaIsi = 4;
LiquidCrystal_I2C lcd(0x27, 20, 4);

OneWire oneWire(ds18b20SensorPin);
DallasTemperature sensors(&oneWire);
GravityTDS gravityTds;
float temperature = 25, tdsValue = 0;
// alat
String idDevice = "Mega";

void setup() {
  Serial.begin(9600); 
  Serial1.begin(9600);
  sensors.begin();  
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);

  gravityTds.setPin(TDSSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();

  pinMode(pinPompaIsi, OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  // Membaca nilai pH dari sensor
  float pHValue = bacaSensorPH();
  Serial.print("pH: ");
  Serial.println(pHValue, 2); 

  // Membaca nilai turbidity dari sensor
  int turbiditynilai = bacaSensorTurbidity();
  Serial.print("Turbidity: ");
  Serial.println(turbiditynilai);

  // Membaca nilai suhu dari sensor DS18B20
  float temperatureValue = bacaSensorSuhu();
  Serial.print("Suhu (°C): ");
  Serial.println(temperatureValue);

    // Membaca nilai TDS dari sensor
  gravityTds.setTemperature(temperatureValue);
  gravityTds.update();
  float tdsValue = gravityTds.getTdsValue();
  Serial.print("TDS (ppm): ");
  Serial.println(tdsValue);

  // Membaca jarak dari sensor ultrasonik
  float jarakValue = bacaSensorUltrasonik();
  Serial.print("tinggi air (cm): ");
  Serial.println(jarakValue);

  // kontrolFuzzy(jarakValue, temperatureValue);
  kontrolPompa(jarakValue);

  //kirim data ke MCU
  Serial1.print(pHValue, 2);
  Serial1.print(",");
  Serial1.print(turbiditynilai);
  Serial1.print(",");
  Serial1.print(temperatureValue);
  Serial1.print(",");
  Serial1.print(tdsValue);
  Serial1.print(",");
  Serial1.println(jarakValue);
  
  lcd.clear();
  tampilkanNilai("Suhu", temperatureValue, 0, 0);
  tampilkanNilai("Tinggi", jarakValue, 0, 1);
  tampilkanNilai("ntu", turbiditynilai, 0, 2);
  tampilkanNilai("ppm", tdsValue, 0, 3);
  tampilkanNilai("pH", pHValue, 0, 4);

  delay(5000); 
}

float bacaSensorPH() {
  int pHValue = analogRead(pHSensorPin);
  double TeganganPh = 5 / 1024.0 * pHValue;
  float nilaiPh = 7.00 + ((pHtujuh - TeganganPh)/pHstep);
  return nilaiPh;
}


int bacaSensorTurbidity() {
  int turbidityValue = analogRead(turbiditySensorPin);
  float voltage = turbidityValue * (5.0 / 1024.0);
  float ntu = -1120.4*square(voltage)+5742.3*voltage-4352.9;
  return ntu;
}

float bacaSensorSuhu() {
  sensors.requestTemperatures();
  float temperatureValue = sensors.getTempCByIndex(0);
  return temperatureValue;
} 

float bacaSensorUltrasonik() {
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTrigPin, LOW);

  long duration = pulseIn(ultrasonicEchoPin, HIGH);
  float distance = (duration * 0.0343) / 2; // Menghitung jarak dalam cm
  if(distance>batasMinPemSensor && distance<jarakSensorDasarAir){
    tinggiAir = jarakSensorDasarAir - distance;
    tinggiAirPrev = tinggiAir;
  }else{
    tinggiAir = tinggiAirPrev;
  }
  return tinggiAir;
}

void kontrolPompa(float jarakValue) {
  const float airRendah = 50.0;
  const float airSedang = 75.0;

  if (jarakValue < airRendah) {
    digitalWrite(pinPompaIsi, LOW);
  } else if (jarakValue >= airRendah && jarakValue < airSedang) {
    digitalWrite(pinPompaIsi, HIGH);
  } else if (jarakValue >= airSedang) {
    digitalWrite(pinPompaIsi, HIGH);
  }
}

void tampilkanNilai(const char* label, float value, int column, int row) {
    lcd.setCursor(column, row);
    lcd.print(label);
    lcd.print(":");
    lcd.print(value);
}
