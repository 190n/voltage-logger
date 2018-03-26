#include <SD.h>

File dataFile;
int samples = 0;

#define LED_PIN 9
#define BATTERY_PIN A0

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
  Serial.begin(9600);
  Serial.print("init SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("failed.");
    blinkError(1);
    while(1);
  }
  Serial.println("done.");
  dataFile = SD.open("voltages.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("error opening voltages.txt");
    blinkError(2);
    while(1);
  }
}

void loop() {
  if (samples >= 1000) return;
  int measurement = analogRead(BATTERY_PIN);
  Serial.println(measurement);
  dataFile.println(measurement);
  samples++;
  if (samples >= 1000) {
    dataFile.close();
    digitalWrite(LED_PIN, HIGH);
  }
  delay(100);
}
