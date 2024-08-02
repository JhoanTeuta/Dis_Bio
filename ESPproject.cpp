#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MI SEGUNDO HOGAR.";
const char* password = "3217555857diana";
const char* serverName = "http://127.0.0.1:1880/posture"; 

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);

  // Conectar a Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Iniciar el sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  String postData = "accelX=" + String(a.acceleration.x) + "&accelY=" + String(a.acceleration.y) + "&accelZ=" + String(a.acceleration.z);
  postData += "&gyroX=" + String(g.gyro.x) + "&gyroY=" + String(g.gyro.y) + "&gyroZ=" + String(g.gyro.z);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);
    Serial.println(httpResponseCode);
    http.end();
  }
  
  delay(1000);
}
