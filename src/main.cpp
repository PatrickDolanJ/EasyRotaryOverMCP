#include <Arduino.h>
#include <EasyRotaryMCP.h>

EasyRotary rotary;
bool clockwise;
int id;
bool print = false;


void setup(){
Serial.begin(115200);
Serial.println("From setup");
rotary.startup();
}


void loop(){
if(rotary.checkInterrupt()){
   print = true;
};
if(print){
id = rotary.getId();
clockwise = rotary.getClockwise();
Serial.println("id: " + String(id) + " clockwise: " + String(clockwise));
print = false;
}
}