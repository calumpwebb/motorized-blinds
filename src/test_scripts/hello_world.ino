// Hello World - Fast LED Blink Test
// XIAO ESP32C6 onboard LED on GPIO 15

#define LED_PIN 15
#define BLINK_DELAY 100  // milliseconds (super fast!)

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello World! Fast blink starting...");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // LED on
  delay(BLINK_DELAY);
  digitalWrite(LED_PIN, LOW);   // LED off
  delay(BLINK_DELAY);
}
