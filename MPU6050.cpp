#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
const int vibradorPin = 4;           // Pin GPIO para el vibrador
const float anguloObjetivo = 160.0;  // Ángulo aproximado para posición vertical
const float tolerancia = 10.0;        // Tolerancia de ±10° aun por evaluar

void setup() {
  Serial.begin(115200);

  // Configurar el pin del vibrador como salida
  pinMode(vibradorPin, OUTPUT);
  digitalWrite(vibradorPin, LOW); // Asegurarse de que el vibrador esté apagado al inicio

  // Iniciar el sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("No se encontró el chip MPU6050");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 inicializado");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calcular el ángulo de inclinación
  float anguloX = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  Serial.print("Ángulo en X: ");
  Serial.println(anguloX);

  // Mostrar los valores de aceleración en todos los ejes
  Serial.print("Aceleración en X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" m/s^2, Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" m/s^2, Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  // Activar el vibrador si el ángulo en X está fuera del rango de 150° a 170°
  if (anguloX < (anguloObjetivo - tolerancia) || anguloX > (anguloObjetivo + tolerancia)) {
    digitalWrite(vibradorPin, HIGH); // Encender el vibrador
    Serial.println("Fuera de posición, vibrador activado");
  } else {
    digitalWrite(vibradorPin, LOW); // Apagar el vibrador
    Serial.println("En posición correcta, vibrador apagado");
  }

  delay(1000); // Intervalo de lectura
}
