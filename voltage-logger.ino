#include <SD.h>

File dataFile;
int threshold = 0;
bool belowThresholdLastTime = false;

#define LED_PIN 9
#define BATTERY_PIN A0
#define BUTTON_PIN 2
#define TRANSISTOR_PIN 3

// measures the arduino's power supply (vcc) in mV
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

// convert reading from Arduino's analog pin (0-1024) into millivolts (0-5000)
// also accounts for voltage divider circuit with two 10-ohm resistors
int readingTomV(int reading) {
  float volts = (reading / 1024.0) * readVcc();
  return (int)(volts * 2);
}

void blinkError(int err) {
  for (int i = 0; i < err; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  delay(200);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(TRANSISTOR_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.print("init SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("failed.");
    blinkError(1);
    while(1) delay(1000);
  }
  Serial.println("done. Enter filename.");
  while(Serial.available() == 0);
  String filename = Serial.readString();
  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile) {
    Serial.print("error opening ");
    Serial.println(filename);
    Serial.println("filename must be no more than 8 characters, then a dot, then an extension of no more than 3 characters (probably txt)");
    blinkError(2);
    while(1) delay(1000);
  }
  while (threshold == 0) {
    Serial.println("Enter threshold voltage (mV). The battery will stop being drained if the voltage is lower than this number.");
    threshold = Serial.parseInt();
  }
  Serial.println(threshold);
  Serial.print("writing to ");
  Serial.print(filename);
  Serial.println(" after 5 seconds");
  delay(5000);
  digitalWrite(TRANSISTOR_PIN, HIGH);
}

void loop() {
  int measurement = analogRead(BATTERY_PIN);
  int mV = readingTomV(measurement);
  Serial.println(mV);
  dataFile.println(mV);
  if (digitalRead(BUTTON_PIN)) stop();
  if (mV < threshold) {
    if (belowThresholdLastTime) stop();
    else belowThresholdLastTime = true;
  } else belowThresholdLastTime = false;
  digitalWrite(LED_PIN, HIGH);
  delay(belowThresholdLastTime ? 750 : 250);
  digitalWrite(LED_PIN, LOW);
  delay(belowThresholdLastTime ? 250 : 750);
}

void stop() {
  dataFile.close();
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(TRANSISTOR_PIN, LOW);
  while(1) delay(1000);
}

