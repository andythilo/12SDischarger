/*
  12S LiPo Discharger (Arduino Firmware)
*/

#define VOLTAGE_PIN A0
#define TEMP_PIN     A1
#define FAN_PIN      5
#define FET_CTRL_PIN 6

const float R1 = 100000.0;
const float R2 = 10000.0;
const float THERMISTOR_BETA = 3950.0;
const float THERMISTOR_NOMINAL = 10000.0;
const float TEMPERATURE_NOMINAL = 25.0;
const float SERIES_RESISTOR = 10000.0;

const float CUTOFF_VOLTAGE = 45.6;
const float FAN_ON_TEMP = 50.0;
const float FAN_OFF_TEMP = 40.0;

void setup() {
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FET_CTRL_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(FET_CTRL_PIN, HIGH);
  Serial.begin(9600);
}

float readBatteryVoltage() {
  int raw = analogRead(VOLTAGE_PIN);
  float voltage = raw * (5.0 / 1023.0);
  return voltage * ((R1 + R2) / R2);
}

float readTemperatureC() {
  int raw = analogRead(TEMP_PIN);
  float voltage = raw * (5.0 / 1023.0);
  float resistance = SERIES_RESISTOR / ((5.0 / voltage) - 1.0);
  float steinhart;
  steinhart = resistance / THERMISTOR_NOMINAL;
  steinhart = log(steinhart);
  steinhart /= THERMISTOR_BETA;
  steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
  return steinhart;
}

void loop() {
  float voltage = readBatteryVoltage();
  float temp = readTemperatureC();

  Serial.print("Voltage: "); Serial.print(voltage); Serial.print(" V	");
  Serial.print("Temp: "); Serial.print(temp); Serial.println(" °C");

  if (voltage <= CUTOFF_VOLTAGE) {
    digitalWrite(FET_CTRL_PIN, LOW);
    Serial.println("Discharge complete.");
  }

  if (temp >= FAN_ON_TEMP) {
    digitalWrite(FAN_PIN, HIGH);
  } else if (temp <= FAN_OFF_TEMP) {
    digitalWrite(FAN_PIN, LOW);
  }

  delay(1000);
}
