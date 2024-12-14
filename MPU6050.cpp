#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>

Adafruit_MPU6050 mpu;
const int vibradorPin = 4;           // Pin GPIO para el vibrador
const float anguloObjetivo = 160.0;  // Ángulo promedio para posición vertical en X
const float tolerancia = 10.0;       // Tolerancia de ±x°

const char* ssid = "Galaxy A127A4E";//MI SEGUNDO HOGAR.";
const char* password = "rryz8141";//3217555857diana";
const char* mqtt_server = "192.168.63.92";//192.168.1.16"; // IP del servidor MQTT

WiFiClient espClient;
PubSubClient client(espClient);

float pitch = 0.0;
float roll = 0.0;
float alpha = 0.98; // Factor de filtro de complemento

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(vibradorPin, OUTPUT);
  digitalWrite(vibradorPin, LOW); // Asegurarse de que el vibrador esté apagado al inicio

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  client.setServer(mqtt_server, 1883);

  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado al broker MQTT");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;
  float gx = g.gyro.x;
  float gy = g.gyro.y;
  float gz = g.gyro.z;

  // Calcular ángulos del acelerómetro
  float pitch_acc = atan2(ay, az) * 180 / PI;
  float roll_acc = atan2(-ax, sqrt(ay * ay + az * az)) * 180 / PI;

  // Integrar giroscopio para obtener ángulos
  float dt = 0.01; // Intervalo de tiempo en segundos
  pitch += gx * dt;
  roll += gy * dt;

  // Aplicar filtro de complemento
  pitch = alpha * pitch + (1 - alpha) * pitch_acc;
  roll = alpha * roll + (1 - alpha) * roll_acc;

  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print(" Roll: ");
  Serial.println(roll);

  // Calcular el ángulo de inclinación en el eje X
  float anguloX = atan2(ay, az) * 180 / PI;
  Serial.print("Ángulo en X: ");
  Serial.println(anguloX);

  // Activar el vibrador si el ángulo en X está fuera del rango de 155° a 165°
  if (anguloX < (anguloObjetivo - tolerancia) || anguloX > (anguloObjetivo + tolerancia)) {
    digitalWrite(vibradorPin, HIGH); // Encender el vibrador
    Serial.println("Fuera de posición, vibrador activado");
  } else {
    digitalWrite(vibradorPin, LOW); // Apagar el vibrador
    Serial.println("En posición correcta, vibrador apagado");
  }

  // Incluir ax, ay, az en la cadena de datos
  String data = "{\"pitch\":" + String(pitch) + ", \"roll\":" + String(roll) + ", \"ax\":" + String(ax) + ", \"ay\":" + String(ay) + ", \"az\":" + String(az) + "}";
  client.publish("postureMonitor/angle", data.c_str());

  delay(100);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado al broker MQTT");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos...");
      delay(5000);
    }
  }
}
