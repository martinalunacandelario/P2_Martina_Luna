#include <Arduino.h>

struct Button { 
  const uint8_t PIN; 
  volatile uint32_t numberKeyPresses; 
  volatile bool pressed; 
}; 
Button button1 = {18, 0, false}; 
void IRAM_ATTR isr() { 
  button1.numberKeyPresses += 1; 
  button1.pressed = true; 
} 
void setup() { 
  Serial.begin(115200); 
  pinMode(button1.PIN, INPUT_PULLUP); 
  attachInterrupt(button1.PIN, isr, FALLING); 
  Serial.println("Inicio de procesador");
} 
void loop() { 
  if (button1.pressed) { 
      Serial.print("Button 1 has been pressed ");
      Serial.print(button1.numberKeyPresses);
      Serial.println(" times");
      button1.pressed = false; 
  } 

  //Detach Interrupt after 1 Minute 
  static uint32_t lastMillis = 0; 
  if (millis() - lastMillis > 60000) { 
    lastMillis = millis(); 
    detachInterrupt(button1.PIN); 
     Serial.println("Interrupt Detached!"); 
  } 
}