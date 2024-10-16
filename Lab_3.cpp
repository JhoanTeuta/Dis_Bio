#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Definir el pin y el tipo de sensor DHT
#define DHTPIN 19       // Cambia el pin según donde conectaste el DHT
#define DHTTYPE DHT11   // O usa DHT22 si ese es tu sensor
#define LED_R 25
#define LED_G 26
#define LED_B 27
#define RELAY_PIN 5
#define BTN_SET 4
#define BTN_UP 23
#define BTN_DOWN 15

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C del LCD

// Variables globales
int tempLimit = 20; // Límite de temperatura inicial
bool setMode = false;

void setup() {
  // Inicializar el sensor DHT
  Serial.begin(9600);
  dht.begin();

  // Inicializar el LCD
  Wire.begin(21, 22);  // SDA en GPIO 21, SCL en GPIO 22 para el ESP32
  lcd.begin(16, 2);    // Inicializa el LCD con 16 columnas y 2 filas
  lcd.backlight();

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BTN_SET, INPUT);
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  
  // Inicialmente apagar el ventilador
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  // Leer la temperatura y la humedad del sensor
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Comprobar si las lecturas son válidas
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Error al leer el sensor DHT");
    return; // Si falla, no continuar
  }

  // Mostrar los valores en el monitor serial
    // Mostrar los datos en el LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print("C H:");
  lcd.print(hum,.1f);
  lcd.print("%");
  
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Humedad: ");
  Serial.print(hum);
  Serial.println(" %");

  cambiarColorLED(temp);

// Controlar el ventilador según el límite de temperatura
  if (temp > tempLimit) {
    digitalWrite(RELAY_PIN, HIGH); // Encender ventilador
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Apagar ventilador
  }

  // Controlar el ajuste del límite de temperatura con pulsadores
  ajustarLimiteTemp();
  delay(2000);  // Esperar 2 segundos entre lecturas



}

void cambiarColorLED(float temp) {
  if (temp < 20) {
    setColor(0, 0, 255); // Azul si es menor a 20°C
  } else if (temp >= 20 && temp <= 30) {
    setColor(0, 255, 0); // Verde entre 20°C y 30°C
  } else {
    setColor(255, 0, 0); // Rojo si es mayor a 30°C
  }
}

// Función para establecer el color del LED RGB
void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

// Función para ajustar el límite de temperatura con los pulsadores
void ajustarLimiteTemp() {
  if (digitalRead(BTN_SET) == HIGH) {
    setMode = !setMode;  // Alternar modo de ajuste
    delay(200);  // Anti-rebote
  }

  if (setMode) {
    if (digitalRead(BTN_UP) == HIGH) {
      tempLimit++;  // Incrementar límite
      delay(200);   // Anti-rebote
    }
    if (digitalRead(BTN_DOWN) == HIGH) {
      tempLimit--;  // Disminuir límite
      delay(200);   // Anti-rebote
    }
  }

  // Mostrar el límite ajustado en el LCD
  lcd.setCursor(0, 1);
  lcd.print("Limite: ");
  lcd.print(tempLimit);
  lcd.print("C       ");
}
