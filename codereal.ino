#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

float GRC;
float distance;
float gyroDegree;
bool isFirstLoopComplete;  
float previousTime;
const int trigPin = 9;
const int echoPin = 10;
long duration;

#define GYRO_Z_OFFSET 16


MPU6050 gyroAccelTemp;
LiquidCrystal_I2C lcd(0x27,20,4);

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Ground Clearance:");
  lcd.setCursor(14,1);
  lcd.print("cm");
  lcd.setCursor(0,2);
  lcd.print("Tilt Angle:");
  lcd.setCursor(14,3);
  lcd.print("DEG");
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  Serial.begin(115200);

  Wire.begin();
  Serial.println("Initializing I2C devices...");
  gyroAccelTemp.initialize();

  Serial.println("Testing device connections...");
  Serial.println(gyroAccelTemp.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  gyroAccelTemp.CalibrateGyro(); 
  Serial.println("Calibration complete");
  gyroAccelTemp.setZGyroOffset(GYRO_Z_OFFSET);

  gyroAccelTemp.CalibrateGyro(6);
  Serial.println("Fine Tuning Complete");
  isFirstLoopComplete = false;
}

void loop() {
  // put your main code here, to run repeatedly:
  float gyroZ = gyroAccelTemp.getRotationZ() / 131.0;
  if (isFirstLoopComplete) {
      float timeForOneLoop = micros() - previousTime;
      
      gyroDegree += gyroZ * timeForOneLoop / 1000000.0;
    }
    
  previousTime = micros();
     
  if (!isFirstLoopComplete) {
      isFirstLoopComplete = true;
  }
  Serial.println(gyroDegree);
  lcd.setCursor(2,3);
  lcd.print(gyroDegree);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  Serial.println(distance);
  GRC = (cos(gyroDegree*(PI/180)))*distance;
  if (GRC<0) {
    GRC = -1*GRC;
  }
  Serial.println(GRC);
  lcd.setCursor(2,1);
  lcd.print(GRC);

  delay(500);
}
