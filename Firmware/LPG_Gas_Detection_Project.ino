#include <HX711_ADC.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
HX711_ADC LoadCell(A0, A1);

int gas_sensor = A2;
int modesw = A3;
int buzzer = 7;
int relay1 = 6;

SoftwareSerial sim(2, 3);
String number1 = "Alert_contact";  // Alert Contact
String number2 = "Booking_Number";  // Booking Number

int flag1 = 0;
int flag2 = 0;
float calibration_factor = 10000.0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  sim.begin(9600);
  delay(5000);  // Allow GSM to initialize
  
  LoadCell.begin();
  LoadCell.start(2000);
  LoadCell.setCalFactor(calibration_factor);

  pinMode(modesw, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(relay1, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(relay1, LOW);

  lcd.setCursor(0, 0);
  lcd.print("GAS BOOKING SYS");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  // Force a test message to check GSM functionality
  sendSMS(number1, "Test: GSM Module Working");
}

void loop() {
  LoadCell.update();
  float weight = LoadCell.getData();
  if (weight < 0) weight = 0;

  int gasr = analogRead(gas_sensor);
  int gas = gasr / 10;  

  Serial.print("Weight: "); Serial.println(weight);
  Serial.print("Gas Level: "); Serial.println(gas);

  // Update LCD only when values change
  static float prevWeight = -1;
  static int prevGas = -1;

  if (weight != prevWeight) {
    lcd.setCursor(0, 0);
    lcd.print("W[g]:       ");
    lcd.setCursor(6, 0);
    lcd.print(weight, 2);
    prevWeight = weight;
  }

  if (gas != prevGas) {
    lcd.setCursor(0, 1);
    lcd.print("Gas:      ");
    lcd.setCursor(5, 1);
    lcd.print(gas);
    prevGas = gas;
  }

  // Gas Leakage Alert
  if (gas >= 3 && flag1 == 0) {
    Serial.println("Gas Leak Detected! Sending SMS...");
    lcd.setCursor(12, 1);
    lcd.print("ALRT");
    digitalWrite(buzzer, HIGH);
    digitalWrite(relay1, HIGH);
    sendSMS(number1, "WARNING: Gas Leakage Detected!");
    flag1 = 1;
  } else if (gas < 3 && flag1 == 1) {
    lcd.setCursor(12, 1);
    lcd.print("SAFE");
    digitalWrite(buzzer, LOW);
    digitalWrite(relay1, LOW);
    flag1 = 0;
  }

  // Low Gas Level Alert
  if (weight <= 2 && flag2 == 0) {
    Serial.println("Gas Level Low! Sending SMS...");
    lcd.setCursor(12, 0);
    lcd.print("LOW");
    sendSMS(number1, "Alert: Gas Level is Low!");
    sendSMS(number2, "Gas Booking Request: Connection No 23LECE201");
    flag2 = 1;
  } else if (weight > 2 && flag2 == 1) {
    lcd.setCursor(12, 0);
    lcd.print("OK ");
    flag2 = 0;
  }

  delay(500);
}

// Function to send SMS with error handling
void sendSMS(String recipient, String message) {
  Serial.println("Attempting to send SMS...");
  
  sim.println("AT+CMGF=1");
  delay(1000);
  Serial.println(readSerial());

  sim.println("AT+CMGS=\"" + recipient + "\"\r");
  delay(1000);
  Serial.println(readSerial());

  sim.println(message);
  delay(500);
  Serial.println(readSerial());

  sim.println((char)26);  // End SMS
  delay(5000);
  Serial.println(readSerial());

  Serial.println("SMS Sent Successfully.");
}

// Function to read GSM responses
String readSerial() {
  int _timeout = 0;
  while (!sim.available() && _timeout < 12000) {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
  return "No response from GSM";
}