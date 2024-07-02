
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines del LED RGB
#define PIN_R 25
#define PIN_G 26
#define PIN_B 27

// Pines de los pulsadores
#define BUTTON1 14
#define BUTTON2 12

// Pin del potenciómetro
#define POT_PIN 13

// Pin del sensor LM35
#define LM35_PIN 4

int mode = 1; // Modo de visualización (1: Temperatura, 2: Potenciómetro)

void setup() {
  Serial.begin(115200);
  
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  
  // Pines del LED RGB
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  
  // Pines de los pulsadores
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  
  // Pin del potenciómetro
  pinMode(POT_PIN, INPUT);
  
  // Pin del sensor LM35
  pinMode(LM35_PIN, INPUT);
}

void loop() {
  // Leer temperatura del sensor LM35
  int lm35Value = analogRead(LM35_PIN);
  float temperature = lm35Value * (3.3 / 4095.0) * 100;
  
  // Leer valor del potenciómetro
  int potValue = analogRead(POT_PIN);
  float voltage = potValue * (3.3 / 4095.0);
  
  // Controlar el color del LED RGB según la temperatura
  if (temperature < 20) {
    setRGBColor(0, 0, 255); // Azul
  } else if (temperature >= 20 && temperature <= 30) {
    setRGBColor(0, 255, 0); // Verde
  } else {
    setRGBColor(255, 0, 0); // Rojo
  }
  
  // Leer pulsadores para cambiar el modo de visualización
  if (digitalRead(BUTTON1) == HIGH) {
    mode = 1;
    delay(200); // Anti-rebote
  }
  if (digitalRead(BUTTON2) == HIGH) {
    mode = 2;
    delay(200); // Anti-rebote
  }
  /*/Opcion para un solo pulsador
  int newValue = digitalRead(buttonPin);
  if(newValue != oldValue){
    if(newValue == HIGH){
      if(mode == 1){
        mode = 2;
      }else{
        mode = 1;
      }
    }
    oldValue = newValue;
  }*/
  
  // Mostrar en el LCD según el modo
  lcd.clear();
  if (mode == 1) {
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");
  } else if (mode == 2) {
    lcd.print("Pot: ");
    lcd.print(voltage);
    lcd.print(" V");
  }
  
  // Enviar datos al monitor serial
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Potentiometer: ");
  Serial.print(voltage);
  Serial.println(" V");
  
  // Cambiar el color del LED RGB
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.toUpperCase();
    if (command == "R") {
      setRGBColor(255, 0, 0);
    } else if (command == "G") {
      setRGBColor(0, 255, 0);
    } else if (command == "B") {
      setRGBColor(0, 0, 255);
    }
  }
  delay(100);
}

void setRGBColor(int r, int g, int b) {
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}
