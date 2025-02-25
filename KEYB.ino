#include <KeyboardBLE.h>
#include <Arduino.h>


// Define a simple structure to hold key data.
struct SimpleKey {
  int outPin;    // Pin to drive the key (set LOW to activate)
  int inPin;     // Pin to read the key (will read LOW when pressed)
  int keyCode;   // The key code to send when the key is pressed
  bool pressed;  // Track the current state of the key
};


SimpleKey keys[5] = {
  {1, 2, ' ', false}, 
  {3, 5, KEY_LEFT_ARROW, false},
  {6, 8, KEY_UP_ARROW, false},
  {9, 10, KEY_DOWN_ARROW, false},
  {11, 13, KEY_RIGHT_ARROW, false},
};



// Function to read battery percentage
int readBatteryPercentage() {
  const float conversion_factor = 3.3f / 4095.0f;  // 12-bit ADC, 3.3V reference
  const float voltage_divider_ratio = 3.0f;          // Voltage divider scales down by 3

  // Enable voltage divider by setting GPIO25 high
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);
  delayMicroseconds(50);  // Minimal delay for voltage stabilization

  // Read from ADC3 (GPIO29)
  int raw_adc = analogRead(29);

  // Disable voltage divider to prevent interference
  digitalWrite(25, LOW);

  // Calculate VSYS voltage
  float vsys_voltage = raw_adc * conversion_factor * voltage_divider_ratio;

  // Map the voltage to a percentage (3.0V = 0%, 4.2V = 100%)
  int percentage = (int)(vsys_voltage * 10);
  return percentage;
}
int bat;
unsigned long previousMillis = 0;

const unsigned long batteryInterval = 30000;  // Update battery every 5 seconds
bool btn(uint8_t pin) {return (digitalRead(pin) == LOW);}



void setup() {
  Serial.begin(9600);
  analogReadResolution(12);  // Set ADC resolution to 12-bit
  bat= readBatteryPercentage();
  Serial.print("Battery Voltage: ");
  Serial.print(bat);
  Serial.println("%");
  KeyboardBLE.setBattery(bat);


  // Configure each key's pins.
  for (int i = 0; i < 5; i++) {
    pinMode(keys[i].outPin, OUTPUT);
    digitalWrite(keys[i].outPin, HIGH);  // Ensure the output is HIGH when idle.
    pinMode(keys[i].inPin, INPUT_PULLUP);  // Enable the internal pullup resistor.
  }
  // Start BLE Keyboard after reading battery
  KeyboardBLE.begin();

  // Set battery percentage in BLE
  KeyboardBLE.setBattery(bat);


  delay(5000); // Optional: delay to allow BLE connection establishment.
    KeyboardBLE.setBattery(bat);

}

void loop() {
    unsigned long currentMillis = millis();

  // Update battery reading and BLE battery service every batteryInterval.
  if (currentMillis - previousMillis >= batteryInterval) {
    previousMillis = currentMillis;
    Serial.print(bat);
    Serial.println("%");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
  }
  // Optimized key scanning loop.
  for (uint8_t i = 0; i < 5; i++) {
    digitalWrite(keys[i].outPin, LOW);
    delayMicroseconds(2);  // Minimal delay for voltage stabilization


    bool isPressed = (digitalRead(keys[i].inPin) == LOW);
    if (isPressed && !keys[i].pressed) {
      KeyboardBLE.press(keys[i].keyCode);
      keys[i].pressed = true;
    } else if (!isPressed && keys[i].pressed) {
      KeyboardBLE.release(keys[i].keyCode);
      keys[i].pressed = false;
    }
    digitalWrite(keys[i].outPin, HIGH);
  }

  delay(5);   // Small delay to help with debouncing.
}
