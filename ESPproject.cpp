#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MI SEGUNDO HOGAR.";
const char* password = "3217555857diana";
String apiKey = "V64QG9ZFV2Y0YHB9";
const char* server = "http://api.thingspeak.com/update";

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  Wire.begin();
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 conectado correctamente");
  } else {
    Serial.println("Error al conectar MPU6050");
  }
}

void loop() {
  if (mpu.testConnection()) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;

    // Algoritmo para detectar caídas
    bool caida = detectarCaida(ax_g, ay_g, az_g);
    
    if (caida) {
      enviarDatos(ax_g, ay_g, az_g);
    }
  }
  delay(1000); // Leer datos cada segundo
}

bool detectarCaida(float ax, float ay, float az) {
  float aceleracion = sqrt(ax * ax + ay * ay + az * az);
  if (aceleracion > 2.5) { // Umbral para detectar caída
    Serial.println("Caída detectada");
    return true;
  }
  return false;
}

void enviarDatos(float ax, float ay, float az) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(ax) + "&field2=" + String(ay) + "&field3=" + String(az);
    http.begin(url.c_str());
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Datos enviados: " + payload);
    } else {
      Serial.println("Error en la conexión");
    }
    http.end();
  }
}
