#include <Arduino.h>
#include <EasyRotaryMCP.h>

EasyRotary rotary;
void setup(){
Serial.begin(115200);
Serial.println("From setup");
rotary.startup();
}


void loop(){
rotary.checkInterrupt();
}