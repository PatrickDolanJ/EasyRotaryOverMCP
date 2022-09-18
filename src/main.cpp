#include <Arduino.h>
#include <EasyRotaryMCP.h>



//const byte b_end_message = 0xff;

//Loop Positions 
int i_loop[8] = {0,0,0,0,0,0,0,0};
//

bool print;

//Function prototypes
void updateUI(bool clockwise, int id);
void sendLoopPositions(bool clockwise, int id);
void sendEndCommand();
//


EasyRotary rotary;;
RotaryData myData;

void setup(){
Serial.begin(115200);
Serial1.begin(9600);
Serial.println("From setup");
rotary.startup(*updateUI);
}


void loop(){
myData = rotary.checkInterrupt();
}
 

 void updateUI(bool clockwise, int id){
   Serial.print("update!");
   sendLoopPositions(clockwise, id);
   }
 
 void sendLoopPositions(bool clockwise, int id){
   int loopArrayPosition = id-1;
   int loopArrayValue = i_loop[loopArrayPosition];
   if(loopArrayPosition!=7){
   if(clockwise){
      loopArrayValue==7 ? i_loop[loopArrayPosition] = 0 : i_loop[loopArrayPosition]++;
   } else {
      loopArrayValue==0 ? i_loop[loopArrayPosition] = 7 : i_loop[loopArrayPosition]--;
   }
   
   Serial1.print("n" + String(loopArrayPosition) + ".val="+String(i_loop[loopArrayPosition]));
   sendEndCommand();
   }
 }

 void sendEndCommand(){
   Serial1.write(0xff);
   Serial1.write(0xff);
   Serial1.write(0xff);
 }