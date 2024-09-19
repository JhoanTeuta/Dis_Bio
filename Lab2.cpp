// Definir pines
const int sensorPin = A0;       // Pin analógico conectado al LM35
const int redPin = 9;           // Pin PWM para el LED rojo del RGB
const int greenPin = 10;        // Pin PWM para el LED verde del RGB
const int bluePin = 11;         // Pin PWM para el LED azul del RGB
const int relayPin = 8;         // Pin digital para controlar el relé

const float thresholdTemp = 30.0; // Umbral de temperatura para activar el ventilador (en °C)

void setup() {
  // Inicializar pines
  pinMode(sensorPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT); 
  pinMode(bluePin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  // Iniciar comunicación serial para monitoreo
  Serial.begin(9600);
}

void loop() {
  // Leer el valor del sensor LM35
  int rawValue = analogRead(sensorPin);
  float voltage = rawValue * (5.0 / 1024.0); // Convertir a voltaje
  float temp = voltage * 100;       // Convertir voltaje a grados Celsius
  Serial.println(temp);	
  // Control del LED RGB
  if (temp < 20.0) {
    setColor(0,0,255);  // Azul para temperaturas menores a 20°C
  } else if (temp >= 20.0 && temp < 25.0) {
    setColor(0,255,0);  // Verde para temperaturas entre 20°C y 25°C
  } else if (temp >= 25.0 && temp < 30.0) {
    setColor(255,255,0); // Amarillo para temperaturas entre 25°C y 30°C
  } else if (temp >= 30.1){
    setColor(255,0,0);   // Rojo para temperaturas superiores a 30°C
    digitalWrite(relayPin, HIGH);  // Activa el relé para encender el ventilador
  }

  // Desactiva el ventilador si la temperatura baja del umbral
  if (temp < thresholdTemp) {
    digitalWrite(relayPin, LOW);   // Apaga el ventilador
  }

  delay(1000);  // Retraso de 1 segundo antes de la siguiente lectura
}

// Función para configurar el color del LED RGB
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);      // Ajustar la intensidad del LED rojo
  analogWrite(greenPin, green);  // Ajustar la intensidad del LED verde
  analogWrite(bluePin, blue);    // Ajustar la intensidad del LED azul
}
