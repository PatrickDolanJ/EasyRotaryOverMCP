#include <Arduino.h>
#include <EasyRotaryMCP.h>

EasyRotary rotary;

RotaryData myData;

void printShit();

bool print;

void setup(){
Serial.begin(115200);
Serial.println("From setup");
rotary.startup();
}


void loop(){
myData = rotary.checkInterrupt();
if(myData.changed){
   print = true;
}

if(print){
   printShit();
}
}

void printShit(){
   Serial.println(String(myData.id) + String(myData.clockwise));
   print = false;
}